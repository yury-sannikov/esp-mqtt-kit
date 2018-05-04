#include "unity.h"
#include "unity_fixture.h"
#include "mqttkit/mqttkit.h"
#include "helpers/abort.h"


TEST_GROUP(Ingestor);

TEST_SETUP(Ingestor)
{
  clearAbort();
}

TEST_TEAR_DOWN(Ingestor)
{
}

TEST(Ingestor, emk_create_configuration_should_return_non_null_config)
{
  TEST_ASSERT_NOT_EQUAL(NULL, emk_create_configuration());
}

TEST(Ingestor, emk_add_gpio_ingestors_fail_on_null_config)
{
  emk_initialize(NULL);
  TEST_ASSERT_EQUAL(1, hasAbort());
}

TEST(Ingestor, emk_add_gpio_ingestors_fail_on_having_same_gpio)
{
  // Create GPIO ingestors and add config (reuse pin)
  emk_ingestor_t *ingestor_data[] = {
      GPIO_INGESTOR("pin1",
        .address=EMK_COMMAND_ADDR(1),
        .config=GPIO_INGESTOR_CFG(
          .gpio = 2,
          .edge = EMK_GPIO_EDGE_POS
        )
      ),
      GPIO_INGESTOR("pin2",
        .address=EMK_STATUS_ADDR(1),
        .config=GPIO_INGESTOR_CFG(
          .gpio = 3,
          .edge = EMK_GPIO_EDGE_BOTH
        )
      ),
      GPIO_INGESTOR("pin3",
        .address=EMK_STATUS_ADDR(1),
        .config=GPIO_INGESTOR_CFG(
          .gpio = 2,
          .edge = EMK_GPIO_EDGE_NEG
        )
      ),
      NULL
  };

  // Create main group and add ingestor
  emk_group_t *groups[] = {
    MAKE_GROUP("main", 1,
      .ingestors = ingestor_data
    ),
    NULL
  };

  // Add groups to the configuration
  emk_config_t config = {
    .groups = groups
  };

  emk_initialize(&config);
}