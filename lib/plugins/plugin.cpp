

#include "plugin.h"

/**
 * Plugin interface
 */

Plugin::Plugin(int _id, const char *_name) : id(_id), name(_name) {}
int Plugin::getId() { return id; }
const char *Plugin::getName() { return name; }
bool Plugin::isEnabled() { return enabled; }
void Plugin::loadPluginSettings(JsonObject s) {
  if (s.containsKey(F("enabled"))) {
    enabled = s[F("enabled")];
  }
}
void Plugin::savePluginSettings(JsonObject s) { s[F("enabled")] = enabled; }
void Plugin::setSystem(System *s) { system = s; }
/**
 * setup
 *
 * called at end of ahoi main setup
 *
 * @param app - pointer to ahoi settings
 */
void Plugin::setup() {}
/**
 * loop
 *
 * called at end of ahoi main loop
 *
 * @param app - pointer to ahoi app
 */
void Plugin::loop() {}
/**
 * inverterCallback
 *
 * will be called at end of Inverter.addValue(...)
 *
 *  @param InverterMessage
 */
void Plugin::inverterCallback(const InverterMessage *message) {}
/**
 * mqttCallback
 *
 * will be called at end of app::cbMqtt
 *
 *  @param MqttMessage
 */
void Plugin::mqttCallback(const MqttMessage *message) {}

void Plugin::internalCallback(std::shared_ptr<PluginMessage> message) {}

bool Plugin::onRequest(JsonObject request, JsonObject response) {
  return false;
}

void Plugin::onMqttSubscribe() {}

void Plugin::onTickerSetup() {}

void Plugin::loadSettings(JsonObject settings) {}

void Plugin::saveSettings(JsonObject settings) {}

void Plugin::subscribeMqtt(char *topic, bool append) {
  if (system) {
    system->subscribeMqtt(this, topic, append);
  }
}

bool Plugin::enqueueMessage(char *topic, char *data, bool append) {
  if (system) {
    return system->enqueueMessage(this, topic, data, append);
  }
  return false;
}

void Plugin::publishMessage(PluginMessage &message) {
  if (system) {
    system->publishMessage(this, message);
  }
}

void Plugin::addTimerCb(PLUGIN_TIMER_INTVAL intvaltype, uint32_t interval,
                        std::function<void(void)> timerCb,
                        const char *timername) {
  if (system) {
    system->addTimerCb(this, timername, intvaltype, interval, timerCb);
  }
}
