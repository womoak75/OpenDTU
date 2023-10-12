#ifndef UNITY_INCLUDE_PRINT_FORMATTED
#define UNITY_INCLUDE_PRINT_FORMATTED
#endif
#include "Mocks.hpp"
#include "unity.h"

#define MAX_NUM_INVERTERS 1

#include "PowercontrolPlugin.h"

class PowerControlTests {
public:
  static void test_PowercontrolLimit(void) {
    TEST_MESSAGE("test_PowercontrolLimit");
    SystemMock testSystem;
    auto pmptr = testSystem.createPowerMessage(800);
    auto mmptr = testSystem.createMeterMessage(400);

    DynamicJsonDocument doc(1024);
    JsonObject config = doc.createNestedObject("config");
    config["enabled"] = true;
    config["meter_serial"] = mmptr->serial;
    config["inverter_serial"] = pmptr->deviceId;

    auto powerControl = testSystem.createPlugin<PowercontrolPlugin>(
        PowercontrolPlugin(), config);

    LimitControlMessage *lmc = nullptr;
    testSystem.cb = [&lmc](const std::shared_ptr<PluginMessage> m) {
      if (m.get()->isMessageType<LimitControlMessage>())
        lmc = (LimitControlMessage *)m.get();
    };

    powerControl->internalCallback(pmptr);
    powerControl->internalCallback(mmptr);
    powerControl->loop();

    TEST_ASSERT_NOT_NULL(lmc);
  }
  static void test_PowercontrolLimitUnknownSender(void) {
    TEST_MESSAGE("test_PowercontrolLimitUnknownSender");
    SystemMock testSystem;
    auto pmptr = testSystem.createPowerMessage(800);
    auto mmptr = testSystem.createMeterMessage(400);

    DynamicJsonDocument doc(1024);
    JsonObject config = doc.createNestedObject("config");
    config["enabled"] = true;
    config["meter_serial"] = mmptr->serial;
    config["inverter_serial"] = pmptr->deviceId;

    auto powerControl = testSystem.createPlugin<PowercontrolPlugin>(
        PowercontrolPlugin(), config);

    LimitControlMessage *lmc = nullptr;
    testSystem.cb = [&lmc](const std::shared_ptr<PluginMessage> m) {
      if (m.get()->isMessageType<LimitControlMessage>())
        lmc = (LimitControlMessage *)m.get();
    };
    mmptr->serial = "unknown";
    powerControl->internalCallback(pmptr);
    powerControl->internalCallback(mmptr);
    powerControl->loop();

    TEST_ASSERT_NULL(lmc);
  }

  static void test_function_should_doAlsoDoBlah(void) {
    // more test stuff
  }
};

int runUnityPowercontrolTests(void) {
  UNITY_BEGIN();
  RUN_TEST(PowerControlTests::test_PowercontrolLimit);
  RUN_TEST(PowerControlTests::test_PowercontrolLimitUnknownSender);
  return UNITY_END();
}

