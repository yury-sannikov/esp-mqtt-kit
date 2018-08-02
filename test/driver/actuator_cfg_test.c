#include "unity.h"
#include "unity_fixture.h"
#include "helpers/common_types.h"
#include "mqttkit/mqttkit.h"
#include "helpers/testing.h"
#include "mqttkit/driver.h"
#include "mqttkit/actuator.h"
#include "mqttkit/logic.h"

TEST_GROUP_RUNNER(ActuatorConfig)
{
  RUN_TEST_CASE(ActuatorConfig, emk_abort_on_unknown_actuator_type);
  RUN_TEST_CASE(ActuatorConfig, failing_on_reuse_the_same_ingestor_pin_for_actuator);
  RUN_TEST_CASE(ActuatorConfig, actuator_process_message_address_match);
  RUN_TEST_CASE(ActuatorConfig, actuator_initial_state_true);
}

emk_task_parameter_block_t parameter_block;

TEST_GROUP(ActuatorConfig);
TEST_SETUP(ActuatorConfig)
{
  clearAbort();
  _gpio_write_clear();
  _gpio_enable_clear();
  parameter_block.irq_block = &__gpio_irq_block;
}

TEST_TEAR_DOWN(ActuatorConfig)
{
}


TEST(ActuatorConfig, emk_abort_on_unknown_actuator_type)
{
  const emk_ingestor_t *ingestor_data[] = {
    NULL
  };

  const emk_actuator_t *actuator_data[] = {
      &(emk_actuator_t) {
          .name="non-existent",
          .type=15,
      },
      NULL
  };

  const emk_group_t *groups[] = {
    MAKE_GROUP("main", 1,
      .ingestors = ingestor_data,
      .actuators = actuator_data
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


TEST(ActuatorConfig, failing_on_reuse_the_same_ingestor_pin_for_actuator)
{
  const emk_ingestor_t *ingestor_data[] = {
      GPIO_INGESTOR("pin1",
        .address=EMK_COMMAND_ADDR(1),
        .config=GPIO_INGESTOR_CFG(
          .gpio = 2,
          .edge = EMK_GPIO_EDGE_POS
        )
      ),
      NULL
  };
  const emk_actuator_t *actuator_data[] = {
      GPIO_ACTUATOR("outpin2",
        .address=EMK_COMMAND_ADDR(1),
        .config=GPIO_ACTUATOR_CFG(
          .gpio = 2
        )
      ),
      NULL
  };

  const emk_group_t *groups[] = {
    MAKE_GROUP("main", 1,
      .ingestors = ingestor_data,
      .actuators = actuator_data
    ),
    NULL
  };

  emk_config_t config = {
    .groups = groups
  };

  _create_gpio_irq_block(&config);
  TEST_ASSERT_EQUAL(1, getAbortsCount());
}

TEST(ActuatorConfig, actuator_initial_state_true) {
  const emk_ingestor_t *ingestor_data[] = {
    NULL
  };

  const emk_actuator_t *actuator_data[] = {
      &(emk_actuator_t) {
          .name="relay",
          .type=ACTUATOR_TYPE_GPIO,
          .address=EMK_COMMAND_ADDR(25),
          .config=GPIO_ACTUATOR_CFG(
            .gpio = 7,
            .initial_state = 0xFF
          )
      },
      NULL
  };

  const emk_group_t *groups[] = {
    MAKE_GROUP("main", 1,
      .ingestors = ingestor_data,
      .actuators = actuator_data
    ),
    NULL
  };

  emk_config_t config = {
    .groups = groups,
    .reserved_pins = 0
  };

  _gpio_write_clear();
  _create_gpio_irq_block(&config);

  TEST_ASSERT_EQUAL(0, hasAbort());
  TEST_ASSERT_EQUAL_INT8(7, _gpio_enable_gpio_num);
  TEST_ASSERT_EQUAL_INT8(GPIO_OUTPUT, _gpio_enable_direction);

  TEST_ASSERT_EQUAL(7, _gpio_write__gpio_num);
  TEST_ASSERT_EQUAL(1, _gpio_write__set);
}

TEST(ActuatorConfig, actuator_process_message_address_match) {

  const emk_ingestor_t *ingestor_data[] = {
    NULL
  };

  const emk_actuator_t *actuator_data[] = {
      &(emk_actuator_t) {
          .name="relay",
          .type=ACTUATOR_TYPE_GPIO,
          .address=EMK_COMMAND_ADDR(25),
          .config=GPIO_ACTUATOR_CFG(
            .gpio = 7,
            .initial_state = 0
          )
      },
      NULL
  };

  const emk_group_t *groups[] = {
    MAKE_GROUP("main", 1,
      .ingestors = ingestor_data,
      .actuators = actuator_data
    ),
    NULL
  };

  emk_config_t config = {
    .groups = groups,
    .reserved_pins = 0
  };

  _gpio_write_clear();
  _create_gpio_irq_block(&config);

  TEST_ASSERT_EQUAL(0, hasAbort());

  // Check enable_gpio call
  TEST_ASSERT_EQUAL_INT8(7, _gpio_enable_gpio_num);
  TEST_ASSERT_EQUAL_INT8(GPIO_OUTPUT, _gpio_enable_direction);

  TEST_ASSERT_EQUAL(7, _gpio_write__gpio_num);
  TEST_ASSERT_EQUAL(0, _gpio_write__set);
  _gpio_write_clear();

  emk_message_t msg_wrong = {
    .address = *EMK_COMMAND_ADDR(1),
    .data = (emk_data_t) {
        .type = DATA_TYPE_GPIO,
        .of.b8 = 1
    }
  };

  _xQueueReceive_pvBuffer = &msg_wrong;
  _xQueueReceive_retval = 1;

  __gpio_irq_block.queue = 1;
  parameter_block.config = &config;
  parameter_block.irq_block = &__gpio_irq_block;
  receiver_task(&parameter_block);
  TEST_ASSERT_EQUAL(0, hasAbort());

  TEST_ASSERT_EQUAL(0, _gpio_write__gpio_num);
  TEST_ASSERT_EQUAL(0, _gpio_write__set);

  emk_message_t msg_okay_on = {
    .address = *EMK_COMMAND_ADDR(25),
    .data = (emk_data_t) {
        .type = DATA_TYPE_B8,
        .of.b8 = 1
    }
  };

  _xQueueReceive_pvBuffer = &msg_okay_on;
  _xQueueReceive_retval = 1;
  receiver_task(&parameter_block);

  TEST_ASSERT_EQUAL(7, _gpio_write__gpio_num);
  TEST_ASSERT_EQUAL(1, _gpio_write__set);
  _gpio_write_clear();

  emk_message_t msg_okay_off = {
    .address = *EMK_COMMAND_ADDR(25),
    .data = (emk_data_t) {
        .type = DATA_TYPE_B8,
        .of.b8 = 0
    }
  };

  _xQueueReceive_pvBuffer = &msg_okay_off;
  _xQueueReceive_retval = 1;
  receiver_task(&parameter_block);

  TEST_ASSERT_EQUAL(7, _gpio_write__gpio_num);
  TEST_ASSERT_EQUAL(0, _gpio_write__set);
  _gpio_write_clear();
}
