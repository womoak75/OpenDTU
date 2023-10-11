
#ifndef UNITY_INCLUDE_PRINT_FORMATTED
#define UNITY_INCLUDE_PRINT_FORMATTED
#endif
#include "Mocks.hpp"
#include "unity.h"

#define MAX_NUM_INVERTERS 1

#include "base/pluginmessagepublisher.h"
#include "base/pluginmessagequeuepublisher.h"
#include "base/pluginmessagequeuepublisher.cpp"

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
    publisher.loop();
  }
};

int runQueueUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(QueueTest::test_priorityqueue);
  return UNITY_END();
}

void setup() {
  // Wait ~2 seconds before the Unity test runner
  // establishes connection with a board Serial interface
  delay(2000);

  runQueueUnityTests();
}
void loop() {}
