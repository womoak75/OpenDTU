
#ifndef UNITY_INCLUDE_PRINT_FORMATTED
#define UNITY_INCLUDE_PRINT_FORMATTED
#endif
#include "Mocks.hpp"
#include "unity.h"

#define MAX_NUM_INVERTERS 1

#include "test_powercontrol.h"
#include "test_queues.h"

void setup() {
  // Wait ~2 seconds before the Unity test runner
  // establishes connection with a board Serial interface
  delay(2000);

  UNITY_BEGIN();
  RUN_TEST(PowerControlTests::test_PowercontrolLimit);
  RUN_TEST(PowerControlTests::test_PowercontrolLimitUnknownSender);
  RUN_TEST(QueueTest::test_priorityqueue);
  RUN_TEST(QueueTest::test_multiqueue);
  RUN_TEST(QueueTest::test_publishisreceiver);
  UNITY_END();
}
void loop() {}
