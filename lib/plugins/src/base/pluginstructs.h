#pragma once

#include "Unit.h"
#include <memory>
#include <optional>
#include <vector>

struct TIMESTAMP {
  unsigned long val;
  TIMESTAMP() { val = 0; }
  TIMESTAMP(unsigned long v) { val = v; }
};
enum DeviceType { UNKNOWN, METER, INVERTER, POWERCONTROLLER };

struct DeviceStruct {
  String serial;
  DeviceType type;
  DeviceStruct(String &s, DeviceType t) : serial(s), type(t) {}
  virtual ~DeviceStruct() {}
  String &getSerial() { return serial; }
  DeviceType getType() { return type; }
  bool isType(DeviceType t) { return (type == t); }
  bool equals(const DeviceStruct &d) { return d.serial.equals(serial); }
};

template <typename T> struct DevicesStruct {
  std::vector<std::shared_ptr<T>> devices;

  std::shared_ptr<T> &operator[](int index) { return devices[index]; }

  template <typename D> void addDevice(D &dev) {
    static_assert(std::is_base_of<T, D>::value, "D must derive from T");
    devices.push_back(std::make_shared<D>(dev));
  }
  template <typename D> std::shared_ptr<D> getDevice(String &serial) {
    static_assert(std::is_base_of<T, D>::value, "D must derive from T");
    for (auto e : devices) {
      if (e.get()->getSerial().equals(serial))
        return std::static_pointer_cast<D>(e);
    }
    return std::shared_ptr<D>(nullptr);
  }

  bool hasDevice(String &serial) {
    bool b = false;
    for (auto e : devices) {
      if (e.get()->getSerial().equals(serial))
        b = true;
      break;
    }
    return b;
  }
  int count() { return devices.size(); }
};

struct PowerDeviceStruct : DeviceStruct {
  float _power = 0;
  bool updated = false;
  Unit unit = Unit::W;
  TIMESTAMP timestamp = 0;
  ;
  PowerDeviceStruct(String &s, DeviceType t)
      : DeviceStruct(s, t), _power(0), updated(false), unit(Unit::W),
        timestamp() {}

  virtual float getPower() { return _power; }
  virtual void setPower(float p) {
    _power = p;
    timestamp.val = millis();
    updated = true;
  }
  void setUnit(Unit u) { unit = u; }
  Unit getUnit() { return unit; }
  TIMESTAMP getTimestamp() { return timestamp; }
  void setTimestamp(TIMESTAMP ts) { timestamp = ts; }
  bool isUpdated() { return updated; }
  void clearUpdated() { updated = false; }
};

struct PowerDevicesStruct : DevicesStruct<PowerDeviceStruct> {
  float getPower() {
    float f = 0;
    for (auto e : devices) {
      f += e.get()->getPower();
    }
    return f;
  }
  bool isUpdated() {
    bool b = false;
    for (auto e : devices) {
      if (e->isUpdated())
        b = true;
      break;
    }
    return b;
  }
  void clearUpdated() {
    for (auto e : devices)
      e->clearUpdated();
  }
};

struct MeterStruct : PowerDeviceStruct {
  MeterStruct(String &s) : PowerDeviceStruct(s, DeviceType::METER) {}
  MeterStruct(String &&s) : PowerDeviceStruct(s, DeviceType::METER) {}
};

struct MetersStruct : DevicesStruct<MeterStruct>, MeterStruct {
  MetersStruct() : MeterStruct("meters") {}
  MetersStruct(String &s) : MeterStruct(s) {}
  MetersStruct(String &&s) : MeterStruct(s) {}
  float getPower() {
    float f = 0;
    for (auto dev : devices) {
      f += dev->getPower();
    }
    return f;
  }
  bool isUpdated() {
    bool b = false;
    for (auto dev : devices) {
      if (dev->isUpdated())
        b = true;
      break;
    }
    return b;
  }
  void clearUpdated() {
    for (auto dev : devices) {
      dev->clearUpdated();
    }
  }
};

struct InverterStruct : PowerDeviceStruct {
  float newLimit = 0;
  TIMESTAMP newLimitTs = 0;
  TIMESTAMP limitTs = 0;
  float limit = 0;
  bool newLimitUpdate = false;
  bool limitUpdate = false;
  InverterStruct(String &s) : PowerDeviceStruct(s, DeviceType::INVERTER) {}
  InverterStruct(String &&s) : PowerDeviceStruct(s, DeviceType::INVERTER) {}
  void setNewLimit(float f) {
    newLimit = f;
    newLimitTs = millis();
    newLimitUpdate = true;
  }
  float getNewLimit() { return newLimit; }
  bool isNewLimitUpdate() { return newLimitUpdate; }
  void clearNewLimitUpdate() { newLimitUpdate = false; }
  void setLimit(float f) {
    limit = f;
    limitUpdate = true;
  }
  void setLimitTs(TIMESTAMP ts) { limitTs = ts; }
  float getLimit() { return limitUpdate; }
  bool isLimitUpdate() { return limitUpdate; }
  void clearLimitUpdate() { limitUpdate = false; }
};

struct InvertersStruct : DevicesStruct<InverterStruct>, InverterStruct {
  InvertersStruct() : InverterStruct("inverters") {}
  InvertersStruct(String &s) : InverterStruct(s) {}
  InvertersStruct(String &&s) : InverterStruct(s) {}
  float getPower() {
    float f = 0;
    for (auto dev : devices) {
      f += dev->getPower();
    }
    return f;
  }
  bool isUpdated() {
    bool b = false;
    for (auto dev : devices) {
      if (dev->isUpdated())
        b = true;
      break;
    }
    return b;
  }
  void clearUpdated() {
    for (auto dev : devices) {
      dev->clearUpdated();
    }
  }
};

struct PowerControlStruct : DeviceStruct {
  InvertersStruct inverters;
  MetersStruct meters;
  int threshold = 0;
  int maxDiffW = 0;
  bool initialized = false;

  PowerControlStruct(String &&s)
      : DeviceStruct(s, DeviceType::POWERCONTROLLER) {}
  PowerControlStruct(String &s)
      : DeviceStruct(s, DeviceType::POWERCONTROLLER) {}
  bool isInitialized() {
    if (!initialized && meters.isUpdated() && inverters.isUpdated())
      initialized = true;
    return initialized;
  }
  bool isUpdated() { return (meters.isUpdated() || inverters.isUpdated()); }
  void clearUpdated() {
    meters.clearUpdated();
    inverters.clearUpdated();
  }
};

struct PowerControlsStruct : DevicesStruct<PowerControlStruct> {
  PowerControlsStruct(String &s) {}
  PowerControlsStruct(String &&s) {}
  std::shared_ptr<PowerControlStruct> getByInverterId(String &serial) {
    auto r = std::shared_ptr<PowerControlStruct>(nullptr);
    for (auto dev : devices) {
      auto inv = dev->inverters.getDevice<InverterStruct>(serial);
      if (inv.get()) {
        r = dev;
      }
    }
    return r;
  }
  std::shared_ptr<PowerControlStruct> getByMeterId(String &serial) {
    auto r = std::shared_ptr<PowerControlStruct>(nullptr);
    for (auto dev : devices) {
      auto inv = dev->meters.getDevice<MeterStruct>(serial);
      if (inv.get()) {
        r = dev;
        break;
      }
    }
    return r;
  }
};
