#include "unity.h"
#include "unity_fixture.h"
#include "helpers/common_types.h"
#include "mqttkit/mqttkit.h"
#include "helpers/testing.h"
#include "mqttkit/driver.h"
#include "mqttkit/actuator.h"
#include "mqttkit/logic.h"
#include "mqttkit/debug.h"
#include "logic/toggleswitch.h"


TEST_GROUP_RUNNER(ToggleswitchLogic)
{
  RUN_TEST_CASE(ToggleswitchLogic, emk_toggleswitch_on_then_toggle);
  RUN_TEST_CASE(ToggleswitchLogic, emk_abort_on_wrong_type_for_TOGGLESWITCH_SET);

}

emk_task_parameter_block_t parameter_block;
static const emk_ingestor_t *ingestor_data[] = {
  NULL
};

static const emk_actuator_t *actuator_data[] = {
  GPIO_ACTUATOR("Controlled LED",
    .address=EMK_COMMAND_ADDR(20),
    .status_address=EMK_STATUS_ADDR(4),
    // Disable initial reporting not to interfere with tests
    .initial_status_timeout = DISABLE_INITIAL_NOTIFICATION_TIMEOUT,
    .config=GPIO_ACTUATOR_CFG(
        .gpio = 3,
        .initial_state = 0
    )
  ),
  NULL
};

static const emk_logic_t *logic_blocks[] = {
  TOGGLESWITCH_BLOCK("toggleswitch", EMK_COMMAND_ADDR(20),
    ((const emk_logic_slot_t* []) {
      TOGGLESWITCH_SLOT_TURNON("turn lamp on", EMK_COMMAND_ADDR(1)),
      TOGGLESWITCH_SLOT_TURNOFF("turn lamp off", EMK_COMMAND_ADDR(2)),
      TOGGLESWITCH_SLOT_TOGGLE("toggle lamp state", EMK_COMMAND_ADDR(3)),
      TOGGLESWITCH_SLOT_SET("set lamp state", EMK_COMMAND_ADDR(4)),
      TOGGLESWITCH_SLOT_FEEDBACK("feedback from actuator", EMK_STATUS_ADDR(4)),
      NULL
    })
  ),
  NULL
};

static const emk_group_t *groups[] = {
  MAKE_GROUP("main", 1,
    .ingestors = ingestor_data,
    .actuators = actuator_data,
    .logic = logic_blocks
  ),
  NULL
};

static emk_config_t config = {
  .groups = groups,
  .reserved_pins = 0
};


TEST_GROUP(ToggleswitchLogic);
TEST_SETUP(ToggleswitchLogic)
{
  clearAbort();
  _xQueueSend_clear();
  __gpio_irq_block.queue = 1;
  _task_tick_count = 0;
  parameter_block.irq_block = &__gpio_irq_block;

  parameter_block.config = &config;
  emk_initialize(&config);

  TEST_ASSERT_EQUAL(0, hasAbort());

}

TEST_TEAR_DOWN(ToggleswitchLogic)
{
}


TEST(ToggleswitchLogic, emk_toggleswitch_on_then_toggle)
{
  TEST_ASSERT_EQUAL(3, _gpio_write__gpio_num);
  TEST_ASSERT_EQUAL(0, _gpio_write__set);
  _gpio_write_clear();


  // Send message to the TOGGLESWITCH_SLOT_TURNON
  test_send_message(&(emk_message_t){
      .address = *EMK_COMMAND_ADDR(1),
      .data = (emk_data_t) {
          .type = DATA_TYPE_B8,
          .of.b8 = 0
      }
  }, &parameter_block);

  // Send message to the Turn Lamp On
  _gpio_write_clear();
  TEST_ASSERT_EQUAL(true, test_process_next(&parameter_block));
  TEST_ASSERT_EQUAL(3, _gpio_write__gpio_num);
  TEST_ASSERT_EQUAL(1, _gpio_write__set);

  // Propagate TOGGLESWITCH_FEEDBACK message from actuator to the toggleswitch
  TEST_ASSERT_EQUAL(true, test_process_next(&parameter_block));

  // Send message to the TOGGLESWITCH_SLOT_TOGGLE
  test_send_message(&(emk_message_t){
      .address = *EMK_COMMAND_ADDR(3),
      .data = (emk_data_t) {
          .type = DATA_TYPE_B8,
          .of.b8 = 0
      }
  }, &parameter_block);

  // Send message to the toggle lamp state
  // Toggle state should clear GPIO
  _gpio_write_clear();
  TEST_ASSERT_EQUAL(true, test_process_next(&parameter_block));
  TEST_ASSERT_EQUAL(3, _gpio_write__gpio_num);
  TEST_ASSERT_EQUAL(0, _gpio_write__set);

  // Propagate TOGGLESWITCH_FEEDBACK message back
  TEST_ASSERT_EQUAL(true, test_process_next(&parameter_block));
  // No more messages
  TEST_ASSERT_EQUAL(false, test_process_next(&parameter_block));

  //----------------------------------------------------------------------------------

  // Send message to the TOGGLESWITCH_SET
  test_send_message(&(emk_message_t){
      .address = *EMK_COMMAND_ADDR(4),
      .data = (emk_data_t) {
          .type = DATA_TYPE_B8,
          .of.b8 = 1
      }
  }, &parameter_block);

  // Send message to the set lamp state
  // Toggle state should clear GPIO
  _gpio_write_clear();
  TEST_ASSERT_EQUAL(true, test_process_next(&parameter_block));
  TEST_ASSERT_EQUAL(3, _gpio_write__gpio_num);
  TEST_ASSERT_EQUAL(1, _gpio_write__set);

  // Propagate TOGGLESWITCH_FEEDBACK message back
  TEST_ASSERT_EQUAL(true, test_process_next(&parameter_block));
  // No more messages
  TEST_ASSERT_EQUAL(false, test_process_next(&parameter_block));

}

TEST(ToggleswitchLogic, emk_abort_on_wrong_type_for_TOGGLESWITCH_SET)
{
  TEST_ASSERT_EQUAL(3, _gpio_write__gpio_num);
  TEST_ASSERT_EQUAL(0, _gpio_write__set);
  _gpio_write_clear();

  // Send bad message to the TOGGLESWITCH_SET
  test_send_message(&(emk_message_t){
      .address = *EMK_COMMAND_ADDR(4),
      .data = (emk_data_t) {
          .type = DATA_TYPE_GPIO
      }
  }, &parameter_block);
  TEST_ASSERT_EQUAL(1, hasAbort());
}