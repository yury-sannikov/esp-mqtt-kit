#include "helpers/abort.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(Ingestor)
{
  RUN_TEST_CASE(Ingestor, emk_create_configuration_should_return_non_null_config);
  RUN_TEST_CASE(Ingestor, emk_add_gpio_ingestors_fail_on_null_config);
  RUN_TEST_CASE(Ingestor, emk_add_gpio_ingestors_fail_on_having_same_gpio);
}