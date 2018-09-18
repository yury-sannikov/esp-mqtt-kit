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
  RUN_TEST_CASE(ToggleswitchLogic, emk_);
}

emk_task_parameter_block_t parameter_block;

TEST_GROUP(ToggleswitchLogic);
TEST_SETUP(ToggleswitchLogic)
{
    clearAbort();
    _xQueueSend_clear();
    __gpio_irq_block.queue = 1;
    _task_tick_count = 0;
    parameter_block.irq_block = &__gpio_irq_block;
}

TEST_TEAR_DOWN(ToggleswitchLogic)
{
}


TEST(ToggleswitchLogic, emk_)
{
  const emk_ingestor_t *ingestor_data[] = {
    NULL
  };

  const emk_actuator_t *actuator_data[] = {
    GPIO_ACTUATOR("Controlled LED",
      .address=EMK_COMMAND_ADDR(20),
      .config=GPIO_ACTUATOR_CFG(
          .gpio = 3,
          .initial_state = 0
      )
    ),
    NULL
  };

  const emk_logic_t *logic_blocks[] = {
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

  const emk_group_t *groups[] = {
    MAKE_GROUP("main", 1,
      .ingestors = ingestor_data,
      .actuators = actuator_data,
      .logic = logic_blocks
    ),
    NULL
  };

  emk_config_t config = {
    .groups = groups,
    .reserved_pins = 0
  };

  parameter_block.config = &config;

  emk_initialize(&config);
  TEST_ASSERT_EQUAL(0, hasAbort());

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

  test_process_next(&parameter_block);

}
