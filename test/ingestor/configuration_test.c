#include "unity.h"
#include "unity_fixture.h"
#include "helpers/common_types.h"
#include "mqttkit/mqttkit.h"
#include "helpers/testing.h"
#include "mqttkit/driver.h"

TEST_GROUP_RUNNER(IngestorConfig)
{
  RUN_TEST_CASE(IngestorConfig, emk_abort_on_unknown_driver_type);
  RUN_TEST_CASE(IngestorConfig, emk_add_gpio_ingestors_fail_on_null_config);
  RUN_TEST_CASE(IngestorConfig, emk_add_gpio_ingestors_fail_on_having_reserved_gpio);
  RUN_TEST_CASE(IngestorConfig, emk_add_gpio_ingestors_fail_on_large_gpio);
  RUN_TEST_CASE(IngestorConfig, emk_add_gpio_ingestors_check_irq_setup);
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
  TEST_ASSERT_EQUAL(2, getAbortsCount());
}

TEST(IngestorConfig, emk_add_gpio_ingestors_fail_on_large_gpio)
{
  // Create GPIO ingestors and add config (reuse pin)
  const emk_ingestor_t *ingestor_data[] = {
      GPIO_INGESTOR("pin1",
        .address=EMK_COMMAND_ADDR(1),
        .config=GPIO_INGESTOR_CFG(
          .gpio = 33,
          .edge = EMK_GPIO_EDGE_POS
        )
      ),
      GPIO_INGESTOR("pin2",
        .address=EMK_COMMAND_ADDR(1),
        .config=GPIO_INGESTOR_CFG(
          .gpio = 16,
          .edge = EMK_GPIO_EDGE_POS
        )
      ),
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
  TEST_ASSERT_EQUAL(2, getAbortsCount());

}

TEST(IngestorConfig, emk_add_gpio_ingestors_check_irq_setup)
{
  const emk_ingestor_t *ingestor_data[] = {
      GPIO_INGESTOR("pin1",
        .address=EMK_COMMAND_ADDR(1),
        .config=GPIO_INGESTOR_CFG(
          .gpio = 1,
          .edge = EMK_GPIO_EDGE_POS
        )
      ),
      GPIO_INGESTOR("pin2",
        .address=EMK_COMMAND_ADDR(1),
        .config=GPIO_INGESTOR_CFG(
          .gpio = 3,
          .edge = EMK_GPIO_EDGE_POS,
          .debounce = 100
        )
      ),
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

  RETAINED_PTR emk_gpio_irq_block_t* iblock = _create_gpio_irq_block(&config);
  TEST_ASSERT_EQUAL_INT16(BIT(1) | BIT(3), iblock->active_pins);
  TEST_ASSERT_EQUAL_INT16(0, iblock->state);
  TEST_ASSERT_EQUAL_INT32(0, iblock->last_irq[0]);
  TEST_ASSERT_EQUAL_INT32(IRQ_LAST_TRIGGERED_DEFAULT_TIME, iblock->last_irq[1]);
  TEST_ASSERT_EQUAL_INT32(0, iblock->last_irq[2]);
  TEST_ASSERT_EQUAL_INT32(IRQ_LAST_TRIGGERED_DEFAULT_TIME, iblock->last_irq[3]);

  TEST_ASSERT_EQUAL_INT32(0, iblock->debouce_values[0]);
  TEST_ASSERT_EQUAL_INT32(50 / portTICK_PERIOD_MS, iblock->debouce_values[1]);
  TEST_ASSERT_EQUAL_INT32(0, iblock->debouce_values[2]);
  TEST_ASSERT_EQUAL_INT32(100 / portTICK_PERIOD_MS, iblock->debouce_values[3]);

  test_clear_gpio_set_interrupt();
  _register_interrupt_handlers(iblock);

  // gpio_set_interrupt should be called twice for 1 and 3 pin
  assert_gpio_set_interrupt(BIT(1) | BIT(3), 2);
}




