
#ifndef UNITY_INCLUDE_PRINT_FORMATTED
#define UNITY_INCLUDE_PRINT_FORMATTED
#endif
#include "Mocks.hpp"
#include "unity.h"

#define MAX_NUM_INVERTERS 1

#include "base/pluginmessagepublisher.h"
#include "base/pluginmessagequeuepublisher.cpp"
#include "base/pluginmessagequeuepublisher.h"

class QueueTest {
public:
  static void test_priorityqueue(void) {

    SystemMock testSystem;
    DynamicJsonDocument doc(1024);
    JsonObject config = doc.createNestedObject("config");
    config["enabled"] = true;

    std::vector<std::unique_ptr<Plugin>> plugins;
    auto p = PluginMock();
    p.loadPluginSettings(config);
    p.setup();
        p.cb = [](const std::shared_ptr<PluginMessage> m) {
      static int count = 0;
      PowerMessage *pm = (PowerMessage *)m.get();
      if (count == 0)
        TEST_ASSERT_EQUAL_FLOAT(2, pm->value);
      else if (count == 1)
        TEST_ASSERT_EQUAL_FLOAT(1, pm->value);
      else if (count == 2)
        TEST_ASSERT_EQUAL_FLOAT(3, pm->value);
      TEST_MESSAGE("mes cb\n");
      count++;
    };
    plugins.push_back(std::make_unique<PluginMock>(p));

    PluginMultiQueuePriorityMessagePublisher publisher(plugins, false);
    TEST_PRINTF("plugin %s: enabled %d!\n", plugins[0].get()->getName(),
                plugins[0].get()->isEnabled());
    auto pmptr = testSystem.createPowerMessage(1);
    // pmptr.get()->setReceiverId(PluginMock::PLUGINID);
    pmptr.get()->setPriority(5);
    publisher.publish(pmptr);
    pmptr = testSystem.createPowerMessage(2);
    // pmptr.get()->setReceiverId(PluginMock::PLUGINID);
    pmptr.get()->setPriority(9);
    publisher.publish(pmptr);
    pmptr = testSystem.createPowerMessage(3);
    // pmptr.get()->setReceiverId(PluginMock::PLUGINID);
    pmptr.get()->setPriority(2);
    publisher.publish(pmptr);
    publisher.loop();
  }

  static void test_multiqueue(void) {

    SystemMock testSystem;
    DynamicJsonDocument doc(1024);
    JsonObject config = doc.createNestedObject("config");
    config["enabled"] = true;

    std::vector<std::unique_ptr<Plugin>> plugins;
    auto p = PluginMock();
    p.loadPluginSettings(config);
    p.setup();
    p.cb = [](const std::shared_ptr<PluginMessage> m) {
      static int count = 0;
      PowerMessage *pm = (PowerMessage *)m.get();
      if (count == 0)
        TEST_ASSERT_EQUAL_FLOAT(1, pm->value);
      else if (count == 1)
        TEST_ASSERT_EQUAL_FLOAT(2, pm->value);
      else if (count == 2)
        TEST_ASSERT_EQUAL_FLOAT(3, pm->value);
      TEST_MESSAGE("mes cb\n");
      count++;
    };
    plugins.push_back(std::make_unique<PluginMock>(p));

    PluginMultiQueueMessagePublisher publisher(plugins, false);
    TEST_PRINTF("plugin %s: enabled %d!\n", plugins[0].get()->getName(),
                plugins[0].get()->isEnabled());
    auto pmptr = testSystem.createPowerMessage(1);
    // pmptr.get()->setReceiverId(PluginMock::PLUGINID);
    pmptr.get()->setPriority(5);
    publisher.publish(pmptr);
    pmptr = testSystem.createPowerMessage(2);
    // pmptr.get()->setReceiverId(PluginMock::PLUGINID);
    pmptr.get()->setPriority(9);
    publisher.publish(pmptr);
    pmptr = testSystem.createPowerMessage(3);
    // pmptr.get()->setReceiverId(PluginMock::PLUGINID);
    pmptr.get()->setPriority(2);
    publisher.publish(pmptr);
    publisher.loop();
  }
};

int runQueueUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(QueueTest::test_priorityqueue);
  RUN_TEST(QueueTest::test_multiqueue);
  return UNITY_END();
}

