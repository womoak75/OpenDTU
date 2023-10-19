#pragma once

#include "avgpowercontrolalgo.hpp"
#include "base/plugin.h"
#include "base/pluginmessages.h"
#include "base/powercontrolalgo.hpp"
#include "defaultpowercontrolalgo.hpp"
#include "messages/limitcontrolmessage.h"
#include "messages/limitmessage.h"
#include "messages/metermessage.h"
#include "messages/mqttmessage.h"
#include "messages/powercontrolmessage.h"
#include "messages/powermessage.h"

class PowercontrolPlugin : public Plugin {

public:
  PowercontrolPlugin()
      : Plugin(3, "powercontrol"), powercontrollers("controllers") {}

  void setup() {
    subscribe<MeterMessage>();
    subscribe<PowerMessage>();
    subscribe<LimitMessage>();
  }
  void loop() {
    for (int i = 0; i < powercontrollers.devices.size(); i++) {
      static unsigned long pccount = 0;
      if (millis() - pccount > 5000) {
        pccount = millis();

        PDebug.printf(PDebugLevel::DEBUG,
                      "powercontrol loop: powercontrollers[%d] ---\n", i);
        PDebug.printf(PDebugLevel::DEBUG, "pc: updated=%d,initialized=%d \n",
                      powercontrollers.devices[i]->isUpdated(),
                      powercontrollers.devices[i]->isInitialized());
        PDebug.printf(PDebugLevel::DEBUG, "meters: size:%d, updated=%d, \n",
                      powercontrollers.devices[i]->meters.count(),
                      powercontrollers.devices[i]->meters.isUpdated());
        PDebug.printf(PDebugLevel::DEBUG, "inverters: size:%d, updated=%d, \n",
                      powercontrollers.devices[i]->inverters.count(),
                      powercontrollers.devices[i]->inverters.isUpdated());
        PDebug.printf(
            PDebugLevel::DEBUG,
            "inverters: limit=%f limitUpdated=%d, newLimit=%f "
            "newLimitUpdated=%d \n",
            powercontrollers.devices[i]->inverters.getLimit(),
            powercontrollers.devices[i]->inverters.isLimitUpdate(),
            powercontrollers.devices[i]->inverters.getNewLimit(),
            powercontrollers.devices[i]->inverters.isNewLimitUpdate());
        /*
        if (powercontrollers.devices[i]->isUpdated()) {
          PDebug.printf(PDebugLevel::DEBUG,
                        "powercontrol update: consumption=%f "
                        "production=%f limit=XX\n",
                        powercontrollers.devices[i]->meters.getPower(),
                        powercontrollers.devices[i]->inverters.getPower());
        } */
      }
      if (powercontrollers.devices[i]->isInitialized()) {
        if (powercontrollers.devices[i]->meters.isUpdated()) {
          powercontrollers.devices[i]->meters.clearUpdated();
          PDebug.printf(PDebugLevel::DEBUG,
                        "powercontrol powercontrollers[%d].update\n", i);
          if (algo->calcLimit(powercontrollers.devices[i])) {
            publishLimit(powercontrollers.devices[i]);
          }
        }
        if (powercontrollers.devices[i]->inverters.isUpdated()) {
          powercontrollers.devices[i]->inverters.clearUpdated();
          // do we have to do something? :/
        }
      }
    }
  }

  void publishLimit(std::shared_ptr<PowerControlStruct> pc) {
    publishLimitMessage(pc);
    publishLimitMqtt(pc);
  }

  void publishLimitMqtt(std::shared_ptr<PowerControlStruct> pc) {
    for (auto inv : pc->inverters.devices) {
      char topic[inv->getSerial().length() + 7];
      char payload[32];
      int len = snprintf(payload, sizeof(payload), "%f", inv->getNewLimit());
      snprintf(topic, sizeof(payload), "%s/updateLimit",
               inv->getSerial().c_str());
      MqttMessage mqtt(getId(), PluginIds::PluginPublish);
      mqtt.setMqtt(topic, (const uint8_t *)payload, len);
      mqtt.appendTopic = true;
      publishMessage(mqtt);
    }
  }

  void publishLimitMessage(std::shared_ptr<PowerControlStruct> pc) {
    for (auto inv : pc->inverters.devices) {
      LimitControlMessage m(*this);
      m.deviceId = inv->getSerial();
      m.limit = inv->getNewLimit();
      m.unit = Unit::W;
      publishMessage(m);
      inv->clearNewLimitUpdate();
    }
  }

  void handleMessage(PowerMessage *m) {
    auto powercontrol = powercontrollers.getByInverterId(m->deviceId);
    if (powercontrol.get()) {
      auto inverter =
          powercontrol->inverters.getDevice<InverterStruct>(m->deviceId);
      inverter->setPower(Units.convert(m->unit, Unit::W, m->value));
      inverter->setTimestamp(m->getTS());
    } else {
      PDebug.printf(PDebugLevel::WARN,
                    "powercontrol inverterId(%s) not configured\n",
                    m->deviceId.c_str());
    }
  }

  void handleMessage(LimitMessage *m) {
    auto powercontrol = powercontrollers.getByInverterId(m->deviceId);
    if (powercontrol.get()) {
      auto inv = powercontrol->inverters.getDevice<InverterStruct>(m->deviceId);
      inv->setLimit(Units.convert(m->unit, Unit::W, m->limit));
      inv->setLimitTs(m->getTS());
    } else {
      PDebug.printf(PDebugLevel::WARN,
                    "powercontrol inverterId(%s) not configured\n",
                    m->deviceId.c_str());
    }
  }

  void handleMessage(MeterMessage *m) {
    auto powercontrol = powercontrollers.getByMeterId(m->serial);
    if (powercontrol.get()) {
      auto inv = powercontrol->meters.getDevice<MeterStruct>(m->serial);
      inv->setPower(Units.convert(m->unit, Unit::W, m->power));
      inv->setTimestamp(m->getTS());
    } else {
      PDebug.printf(PDebugLevel::WARN,
                    "powercontrol meterserial(%s) not configured\n",
                    m->serial.c_str());
    }
  }

  void internalCallback(std::shared_ptr<PluginMessage> message) {
    if (message->isMessageType<PowerMessage>()) {
      PowerMessage *m = (PowerMessage *)message.get();
      handleMessage(m);
    } else if (message->isMessageType<MeterMessage>()) {
      MeterMessage *m = (MeterMessage *)message.get();
      handleMessage(m);
    } else if (message->isMessageType<LimitMessage>()) {
      LimitMessage *m = (LimitMessage *)message.get();
      handleMessage(m);
    }
  }

  void initPowercontrol(const JsonArray &obj) {
    // [{"id":"pc2","meters":[{"id":"1234"]},"inverters":[{"id":"1188484848"],"threshold":20,"maxdiffw":20,"algo":"default"}]
    PDebug.printf(PDebugLevel::INFO, "powercontrol init size:%d\n", obj.size());
    powercontrollers.clear();
    for (int i = 0; i < obj.size(); i++) {
      JsonObject pc = obj[i];

      PowerControlStruct powercontrol(pc[F("id")].as<String>());
      JsonArray inverters = pc[F("inverters")];
      for (auto i : inverters) {
        InverterStruct inverter(i[F("id")].as<String>());
        powercontrol.inverters.addDevice(inverter);
      }
      JsonArray meters = pc[F("meters")];
      for (auto m : meters) {
        MeterStruct meter(m[F("id")].as<String>());
        powercontrol.meters.addDevice(meter);
      }
      powercontrol.threshold = pc[F("threshold")];
      powercontrol.maxDiffW = pc[F("maxdiffw")];
      powercontrol.algo = pc[F("algo")].as<String>();
      powercontrollers.addDevice(powercontrol);
    }
  }
  void saveSettings(JsonObject &settings) {
    JsonObject pc = settings.createNestedObject(F("config"));
    JsonArray pcnode = pc.createNestedArray(F("pc"));
    for (int i = 0; i < powercontrollers.count(); i++) {
      auto pc = powercontrollers[i];
      auto node = pcnode.createNestedObject();
      node[F("id")] = pc->serial;
      node[F("threshold")] = pc->threshold;
      node[F("maxdiffw")] = pc->maxDiffW;
      node[F("algo")] = pc->algo;
      auto meters = node.createNestedArray(F("meters"));
      for (int j = 0; j < pc->meters.count(); j++) {
        auto meter = meters.createNestedObject();
        meter[F("id")].set(pc->meters[j]->serial);
      }
      auto inverters = node.createNestedArray(F("inverters"));
      for (int j = 0; j < pc->inverters.count(); j++) {
        auto inverter = inverters.createNestedObject();
        inverter[F("id")].set(pc->inverters[j]->serial);
      }
    }
  }
  void loadSettings(JsonObject &settings) {
    if (settings.containsKey(F("config"))) {
      JsonObject root = settings[F("config")];
      JsonArray node = root[F("pc")];
      initPowercontrol(node);
    }
  }

private:
  String inverter_serial = "";
  String meter_serial = "";
  int threshold = 20;
  int maxDiffW = 40;
  // powercontrolstruct powercontrol;
  PowerControlsStruct powercontrollers;
  PowercontrolAlgo *algo = new DefaultPowercontrolAlgo();
};
