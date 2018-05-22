#include "unity.h"
#include "unity_fixture.h"
#include "helpers/common_types.h"
#include "mqttkit/mqttkit.h"
#include "helpers/abort.h"


TEST_GROUP_RUNNER(IngestorConfig)
{
  RUN_TEST_CASE(IngestorConfig, emk_abort_on_unknown_driver_type);
  RUN_TEST_CASE(IngestorConfig, emk_add_gpio_ingestors_fail_on_null_config);
  RUN_TEST_CASE(IngestorConfig, emk_add_gpio_ingestors_fail_on_having_reserved_gpio);
}

TEST_GROUP(IngestorConfig);
TEST_SETUP(IngestorConfig)
{
  clearAbort();
}

TEST_TEAR_DOWN(IngestorConfig)
{
}

TEST(IngestorConfig, emk_add_gpio_ingestors_fail_on_null_config)
{
  _create_gpio_irq_block(NULL);
  TEST_ASSERT_EQUAL(1, hasAbort());
}


TEST(IngestorConfig, emk_abort_on_unknown_driver_type)
{
  const emk_ingestor_t *ingestor_data[] = {
      &(emk_ingestor_t) {
          .name="non-existent", 
          .type=15,
      },
      NULL
  };

  const emk_group_t *groups[] = {
    MAKE_GROUP("main", 1,
      .ingestors = ingestor_data
    ),
    NULL
  };

  emk_config_t config = {
    .groups = groups,
    .reserved_pins = 0
  };
  _create_gpio_irq_block(&config);
  TEST_ASSERT_EQUAL(1, hasAbort());
  
}


TEST(IngestorConfig, emk_add_gpio_ingestors_fail_on_having_reserved_gpio)
{
  // Create GPIO ingestors and add config (reuse pin)
  const emk_ingestor_t *ingestor_data[] = {
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
  const emk_group_t *groups[] = {
    MAKE_GROUP("main", 1,
      .ingestors = ingestor_data
    ),
    NULL
  };

  // Add groups to the configuration
  emk_config_t config = {
    .groups = groups,
    .reserved_pins = BIT(2)
  };

  _create_gpio_irq_block(&config);
  TEST_ASSERT_EQUAL(2, hasAborts(2));

}