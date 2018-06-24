#include <string.h>
#include "unity.h"
#include "unity_fixture.h"
#include "helpers/common_types.h"
#include "mqttkit/mqttkit.h"
#include "helpers/testing.h"
#include "mqttkit/driver.h"
#include "mqttkit/logic.h"

TEST_GROUP_RUNNER(RECEIVER_TASK)
{
  RUN_TEST_CASE(RECEIVER_TASK, fail_if_no_parameter_block);
  RUN_TEST_CASE(RECEIVER_TASK, fail_if_no_queue_set);
  RUN_TEST_CASE(RECEIVER_TASK, middleware_should_process_system_message);

}

emk_task_parameter_block_t parameter_block;


TEST_GROUP(RECEIVER_TASK);
TEST_SETUP(RECEIVER_TASK)
{
  clearAbort();
  _xQueueSendFromISR_clear();

  parameter_block.irq_block = &__gpio_irq_block;
}

TEST_TEAR_DOWN(RECEIVER_TASK)
{
}

TEST(RECEIVER_TASK, fail_if_no_parameter_block) {
    receiver_task(NULL);
    TEST_ASSERT_EQUAL(1, getAbortsCount());
}

TEST(RECEIVER_TASK, fail_if_no_queue_set) {
    receiver_task(&parameter_block);
    TEST_ASSERT_EQUAL(1, getAbortsCount());
}

TEST(RECEIVER_TASK, middleware_should_process_system_message) {
    const emk_ingestor_t *ingestor_data[] = {
        GPIO_INGESTOR("pin1",
            .address=EMK_COMMAND_ADDR(1),
            .config=GPIO_INGESTOR_CFG(
                .gpio = 1,
                .edge = EMK_GPIO_EDGE_POS
            )
        ),
        GPIO_INGESTOR("pin2",
            .address=EMK_COMMAND_ADDR(31),
            .config=GPIO_INGESTOR_CFG(
                .gpio = 3,
                .edge = EMK_GPIO_EDGE_BOTH,
                .debounce = 100
            )
        ),
        NULL
    };

    const emk_group_t *groups[] = {
        MAKE_GROUP("main", 2,
            .ingestors = ingestor_data
        ),
        NULL
    };

    emk_config_t config = {
        .groups = groups,
        .reserved_pins = 0
    };

    __gpio_irq_block.queue = 1;
    parameter_block.config = &config;

    emk_message_t msg = {
        .address = EMK_SYS_MIDDLEWARE_ADDR(DRIVER_TYPE_INGESTOR),
        .data = (emk_data_t) {
            .type = DATA_TYPE_GPIO,
            .of.gpio = (emk_gpio_data_t) {
                .gpio_num = 3,
                .gpio_val = 1
            }
        }
    };
    _xQueueReceive_pvBuffer = &msg;
    _xQueueReceive_retval = 1;
    receiver_task(&parameter_block);

    emk_message_t msg_converted = {
        .address = EMK_GROUP_COMMAND_ADDR(31, 2),
        .data = (emk_data_t) {
            .type = DATA_TYPE_B8,
            .of.b8 = msg.data.of.gpio.gpio_val
        }
    };

    TEST_ASSERT_EQUAL_MEMORY(&msg_converted.address, &_xQueueSendFromISR_msg.address, sizeof(emk_address_t));
    TEST_ASSERT_EQUAL_MEMORY(&msg_converted.data, &_xQueueSendFromISR_msg.data, sizeof(emk_data_t));

    _xQueueSendFromISR_clear();

    // A message for non-confgiured GPIO should not result in any message generation
    emk_message_t msg2 = {
        .address = EMK_SYS_MIDDLEWARE_ADDR(DRIVER_TYPE_INGESTOR),
        .data = (emk_data_t) {
            .type = DATA_TYPE_GPIO,
            .of.gpio = (emk_gpio_data_t) {
                .gpio_num = 2,
                .gpio_val = 1
            }
        }
    };
    _xQueueReceive_pvBuffer = &msg2;
    _xQueueReceive_retval = 1;
    receiver_task(&parameter_block);

    emk_message_t zeroMessage;
    memset(&zeroMessage, 0, sizeof(emk_message_t));

    TEST_ASSERT_EQUAL_MEMORY(&zeroMessage, &_xQueueSendFromISR_msg, sizeof(emk_message_t));

}
