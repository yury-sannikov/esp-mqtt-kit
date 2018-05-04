#include "helpers/abort.h"
#include "unity_fixture.h"

static void RunAllTests(void)
{
  RUN_TEST_GROUP(Ingestor);
}

int main(int argc, const char * argv[])
{
  return UnityMain(argc, argv, RunAllTests);
}
