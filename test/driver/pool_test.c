#include "unity.h"
#include "unity_fixture.h"
#include "helpers/common_types.h"
#include "mqttkit/mqttkit.h"
#include "helpers/testing.h"
#include "mqttkit/driver.h"
#include "mqttkit/actuator.h"
#include "mqttkit/logic.h"
#include "mqttkit/debug.h"


TEST_GROUP_RUNNER(DriverPool)
{
  RUN_TEST_CASE(DriverPool, actuator_send_initial_data_on_pool);
}

emk_task_parameter_block_t parameter_block;

TEST_GROUP(DriverPool);
TEST_SETUP(DriverPool)
{
  clearAbort();
  _gpio_write_clear();
  _gpio_enable_clear();
  parameter_block.irq_block = &__gpio_irq_block;
}

TEST_TEAR_DOWN(DriverPool)
{
}


TEST(DriverPool, actuator_send_initial_data_on_pool)
{
  const emk_ingestor_t *ingestor_data[] = {
    NULL
  };

  const emk_actuator_t *actuator_data[] = {
      &(emk_actuator_t) {
          .name="relay",
          .type=ACTUATOR_TYPE_GPIO,
          .address=EMK_COMMAND_ADDR(25),
          .status_address=EMK_STATUS_ADDR(25),
          .initial_status_timeout = 500,
          .config=GPIO_ACTUATOR_CFG(
            .gpio = 7,
            .initial_state = 0xF
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

  _task_tick_count = 5;

  _gpio_write_clear();
  _create_gpio_irq_block(&config);

  TEST_ASSERT_EQUAL(0, hasAbort());

  // Check enable_gpio call
  TEST_ASSERT_EQUAL_INT8(7, _gpio_enable_gpio_num);
  TEST_ASSERT_EQUAL_INT8(GPIO_OUTPUT, _gpio_enable_direction);

  TEST_ASSERT_EQUAL(7, _gpio_write__gpio_num);
  TEST_ASSERT_EQUAL(1, _gpio_write__set);

  _gpio_write_clear();
  _xQueueSend_clear();

  __gpio_irq_block.queue = 1;
  parameter_block.config = &config;
  parameter_block.irq_block = &__gpio_irq_block;

  // Pool drivers with the same tick count. Drver should not deliver any messages
  _gpio_read_value = true;
  _task_tick_count = 5;
  emk_pool_drivers(&config);
  TEST_ASSERT_EQUAL(0, _xQueueSend_buff_idx);

  // (54 - 5) * portTICK_PERIOD_MS(10) < initial_status_timeout(500) -> should not trigger
  _task_tick_count = 54;
  emk_pool_drivers(&config);
  TEST_ASSERT_EQUAL(0, _xQueueSend_buff_idx);

  // Should trigger message
  _task_tick_count = 55;
  emk_pool_drivers(&config);
  TEST_ASSERT_EQUAL(1, _xQueueSend_buff_idx);

  // Message should match
  emk_message_t msg_reported_status = {
      .address = *EMK_GROUP_STATUS_ADDR(25, 1),
      .data = (emk_data_t) {
          .type = DATA_TYPE_GPIO,
          .of.gpio.gpio_num = 7,
          .of.gpio.gpio_val = 0xF
      }
  };
  TEST_ASSERT_EQUAL_MSG(msg_reported_status, _xQueueSend_buff[0]);
}
