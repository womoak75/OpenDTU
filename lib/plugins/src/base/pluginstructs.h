#pragma once

#include "Unit.h"
#include <memory>
#include <optional>
#include <vector>

struct TIMESTAMP {
  unsigned long val = 0;
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
};

struct PowerDeviceStruct : DeviceStruct {
  float _power;
  bool updated;
  Unit unit;
  TIMESTAMP timestamp;
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
  Unit getUnit() { return unit; }
  TIMESTAMP getTimestamp() { return timestamp; }
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
    for (auto e : devices) {
      f += e.get()->getPower();
    }
    return f;
  }
};

struct InverterStruct : PowerDeviceStruct {
  InverterStruct(String &s) : PowerDeviceStruct(s, DeviceType::INVERTER) {}
  InverterStruct(String &&s) : PowerDeviceStruct(s, DeviceType::INVERTER) {}
};

struct InvertersStruct : DevicesStruct<InverterStruct>, InverterStruct {
  InvertersStruct() : InverterStruct("inverters") {}
  InvertersStruct(String &s) : InverterStruct(s) {}
  InvertersStruct(String &&s) : InverterStruct(s) {}
  float getPower() {
    float f = 0;
    for (auto e : devices) {
      f += e.get()->getPower();
    }
    return f;
  }
};

struct PowerControlStruct : DeviceStruct {
  InvertersStruct inverters;
  MetersStruct meters;
  float limit;
  float newLimit;
  TIMESTAMP limitTs;
  int threshold;
  int maxDiffW;
  PowerControlStruct(String &s) : DeviceStruct(s, DeviceType::POWERCONTROLLER) {}
  PowerControlStruct(String &&s) : DeviceStruct(s, DeviceType::POWERCONTROLLER) {}
};

struct PowerControlsStruct : DevicesStruct<PowerControlStruct> {
  PowerControlsStruct(String &s) {}
  PowerControlsStruct(String &&s){}
};

