#include "helpers/testing.h"
#include "unity_fixture.h"

static void RunAllTests(void)
{
  RUN_TEST_GROUP(IngestorConfig);
  RUN_TEST_GROUP(GPIO_ISR);
  RUN_TEST_GROUP(RECEIVER_TASK);
  RUN_TEST_GROUP(ActuatorConfig);
}

int main(int argc, const char * argv[])
{
  return UnityMain(argc, argv, RunAllTests);
}
