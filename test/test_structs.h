
#ifndef UNITY_INCLUDE_PRINT_FORMATTED
#define UNITY_INCLUDE_PRINT_FORMATTED
#endif
#include "Mocks.hpp"
#include "unity.h"

#define MAX_NUM_INVERTERS 1

#include "base/pluginstructs.h"

class StructTest {
public:
  static void test_structs(void) {
    TEST_MESSAGE("test structs");
    DevicesStruct<DeviceStruct> devices;
    MetersStruct meterdevices;
    InvertersStruct inverterdevices;
    PowerDevicesStruct powerdevices;
    MeterStruct m1("m1");
    m1.setPower(1);
    MeterStruct m2("m2");
    m2.setPower(2);
    InverterStruct s1("s1");
    s1.setPower(23);
    InverterStruct s2("s2");
    s2.setPower(42);
    PowerControlStruct pwc("pwc");
    pwc.limit = 123;
    devices.addDevice(m1);
    devices.addDevice(m2);
    devices.addDevice(s1);
    devices.addDevice(s2);
    devices.addDevice(pwc);
    powerdevices.addDevice(m1);
    powerdevices.addDevice(m2);
    powerdevices.addDevice(s1);
    powerdevices.addDevice(s2);
    meterdevices.addDevice(m1);
    meterdevices.addDevice(m2);
    inverterdevices.addDevice(s1);
    inverterdevices.addDevice(s2);
    TEST_MESSAGE("devices ***");
    for (auto m : devices.devices) {
      print(m);
    }
    TEST_MESSAGE("meters ***");
    for (auto m : meterdevices.devices) {
      print(m);
    }
    TEST_MESSAGE("inverter ***");
    for (auto m : inverterdevices.devices) {
      print(m);
    }
    TEST_MESSAGE("powerdevices ***");
    for (auto m : powerdevices.devices) {
      print(m);
    }
    // devices
    auto met = devices.getDevice<MeterStruct>(m1.getSerial());
    TEST_ASSERT_EQUAL_STRING(m1.getSerial().c_str(),
                             met.get()->getSerial().c_str());
    TEST_ASSERT_EQUAL_FLOAT(m1.getPower(), met.get()->getPower());
    met = devices.getDevice<MeterStruct>(m2.getSerial());
    TEST_ASSERT_EQUAL_STRING(m2.getSerial().c_str(),
                             met.get()->getSerial().c_str());
    TEST_ASSERT_EQUAL_FLOAT(m2.getPower(), met.get()->getPower());
    auto inv = devices.getDevice<MeterStruct>(s1.getSerial());
    TEST_ASSERT_EQUAL_STRING(s1.getSerial().c_str(),
                             inv.get()->getSerial().c_str());
    TEST_ASSERT_EQUAL_FLOAT(s1.getPower(), inv.get()->getPower());
    inv = devices.getDevice<MeterStruct>(s2.getSerial());
    TEST_ASSERT_EQUAL_STRING(s2.getSerial().c_str(),
                             inv.get()->getSerial().c_str());
    TEST_ASSERT_EQUAL_FLOAT(s2.getPower(), inv.get()->getPower());
    // meter
    auto mmet = devices.getDevice<MeterStruct>(m1.getSerial());
    TEST_ASSERT_EQUAL_STRING(m1.getSerial().c_str(),
                             mmet.get()->getSerial().c_str());
    TEST_ASSERT_EQUAL_FLOAT(m1.getPower(), mmet.get()->getPower());
    mmet = devices.getDevice<MeterStruct>(m2.getSerial());
    TEST_ASSERT_EQUAL_STRING(m2.getSerial().c_str(),
                             mmet.get()->getSerial().c_str());
    TEST_ASSERT_EQUAL_FLOAT(m2.getPower(), mmet.get()->getPower());
    // inverter
    auto iinv = devices.getDevice<MeterStruct>(s1.getSerial());
    TEST_ASSERT_EQUAL_STRING(s1.getSerial().c_str(),
                             iinv.get()->getSerial().c_str());
    TEST_ASSERT_EQUAL_FLOAT(s1.getPower(), iinv.get()->getPower());
    iinv = devices.getDevice<MeterStruct>(s2.getSerial());
    TEST_ASSERT_EQUAL_STRING(s2.getSerial().c_str(),
                             iinv.get()->getSerial().c_str());
    TEST_ASSERT_EQUAL_FLOAT(s2.getPower(), iinv.get()->getPower());
    // powerdevice
    auto pmet = devices.getDevice<MeterStruct>(m1.getSerial());
    TEST_ASSERT_EQUAL_STRING(m1.getSerial().c_str(),
                             pmet.get()->getSerial().c_str());
    TEST_ASSERT_EQUAL_FLOAT(m1.getPower(), pmet.get()->getPower());
    pmet = devices.getDevice<MeterStruct>(m2.getSerial());
    TEST_ASSERT_EQUAL_STRING(m2.getSerial().c_str(),
                             pmet.get()->getSerial().c_str());
    TEST_ASSERT_EQUAL_FLOAT(m2.getPower(), pmet.get()->getPower());
    auto pinv = devices.getDevice<MeterStruct>(s1.getSerial());
    TEST_ASSERT_EQUAL_STRING(s1.getSerial().c_str(),
                             pinv.get()->getSerial().c_str());
    TEST_ASSERT_EQUAL_FLOAT(s1.getPower(), pinv.get()->getPower());
    pinv = devices.getDevice<MeterStruct>(s2.getSerial());
    TEST_ASSERT_EQUAL_STRING(s2.getSerial().c_str(),
                             pinv.get()->getSerial().c_str());
    TEST_ASSERT_EQUAL_FLOAT(s2.getPower(), pinv.get()->getPower());
  }
  static void print(std::shared_ptr<DeviceStruct> m) {
    if (m->isType(METER)) {
      MeterStruct *ms = (MeterStruct *)(m.get());
      TEST_PRINTF("meter: %s,%f,%d", ms->getSerial().c_str(), ms->getPower(),
                  ms->getType());
    } else if (m->isType(INVERTER)) {
      InverterStruct *ms = (InverterStruct *)(m.get());
      TEST_PRINTF("inverter: %s,%f,%d", ms->getSerial().c_str(), ms->getPower(),
                  ms->getType());
    } else if (m->isType(POWERCONTROLLER)) {
      PowerControlStruct *ms = (PowerControlStruct *)(m.get());
      TEST_PRINTF("powercontrol: %s,%d", ms->getSerial().c_str(),
                  ms->getType());
    } else {
      TEST_PRINTF("unknown: %s,%d\n", m->getSerial().c_str(), m->getType());
    }
  }
};

int runStructUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(StructTest::test_structs);
  return UNITY_END();
}
