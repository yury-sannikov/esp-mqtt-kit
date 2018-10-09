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
  RUN_TEST_CASE(RECEIVER_TASK, middleware_same_pin_integration_with_irq);
}

emk_task_parameter_block_t parameter_block;


TEST_GROUP(RECEIVER_TASK);
TEST_SETUP(RECEIVER_TASK)
{
  clearAbort();
  _xQueueSend_clear();

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
        .address = *EMK_GROUP_COMMAND_ADDR(31, 2),
        .data = (emk_data_t) {
            .type = DATA_TYPE_B8,
            .of.b8 = msg.data.of.gpio.gpio_val
        }
    };

    TEST_ASSERT_EQUAL_MSG(msg_converted, _xQueueSend_buff[0]);

    _xQueueSend_clear();

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

    TEST_ASSERT_EQUAL_MSG(zeroMessage, _xQueueSend_buff[0]);

}

TEST(RECEIVER_TASK, middleware_same_pin_integration_with_irq) {
    const emk_ingestor_t *ingestor_data[] = {
        GPIO_INGESTOR("pin1 pos",
            .address=EMK_COMMAND_ADDR(1),
            .config=GPIO_INGESTOR_CFG(
                .gpio = 1,
                .edge = EMK_GPIO_EDGE_POS
            )
        ),
        GPIO_INGESTOR("pin1 neg",
            .address=EMK_COMMAND_ADDR(2),
            .config=GPIO_INGESTOR_CFG(
                .gpio = 1,
                .edge = EMK_GPIO_EDGE_NEG,
                .debounce = 0
            )
        ),
        GPIO_INGESTOR("pin1 both",
            .address=EMK_COMMAND_ADDR(3),
            .config=GPIO_INGESTOR_CFG(
                .gpio = 1,
                .edge = EMK_GPIO_EDGE_BOTH,
                .debounce = 0
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

    __gpio_irq_block.queue = 1;
    parameter_block.config = &config;

    __gpio_irq_block.active_pins = 0xFFFF;
    __gpio_irq_block.last_irq[1] = 0;
    __gpio_irq_block.debouce_values[1] = 0;
    __gpio_irq_block.pos_edge = 0x2; // Pin 1
    __gpio_irq_block.neg_edge = 0x2; // Pin 1

    _xQueueSendFromISR_clear();

    _task_tick_count = 100;
    _gpio_read_value = true;
    gpio_ingestor_interrupt_handler(1);

    _task_tick_count = 200;
    _gpio_read_value = false;
    gpio_ingestor_interrupt_handler(1);

    // GPIO ingestor interrupt should generate 2 messages
    TEST_ASSERT_EQUAL(2, _xQueueSendFromISR_msg_buff_idx);

    // Check 2 system messages has been generated. One for positive edge on GPIO1 andother for negative edge for same GPIO
    emk_message_t msg_irq = {
        .address = EMK_SYS_MIDDLEWARE_ADDR(DRIVER_TYPE_INGESTOR),
        .data = (emk_data_t) {
            .type = DATA_TYPE_GPIO,
            .of.gpio = (emk_gpio_data_t) {
                .gpio_num = 1,
                .gpio_val = 1
            }
        }
    };

    TEST_ASSERT_EQUAL_MSG(msg_irq, _xQueueSendFromISR_msg_buff[0]);

    emk_message_t msg_irq2 = {
        .address = EMK_SYS_MIDDLEWARE_ADDR(DRIVER_TYPE_INGESTOR),
        .data = (emk_data_t) {
            .type = DATA_TYPE_GPIO,
            .of.gpio = (emk_gpio_data_t) {
                .gpio_num = 1,
                .gpio_val = 0
            }
        }
    };

    TEST_ASSERT_EQUAL_MSG(msg_irq2, _xQueueSendFromISR_msg_buff[1]);

    emk_message_t msg_replay[2];
    memcpy(&msg_replay, &_xQueueSendFromISR_msg_buff, sizeof(emk_message_t[2]));

    // Forward these 2 messages to the middleware
    _xQueueSend_clear();

    _xQueueReceive_pvBuffer = &msg_replay[0];
    _xQueueReceive_retval = 1;
    receiver_task(&parameter_block);
    _xQueueReceive_pvBuffer = &msg_replay[1];
    _xQueueReceive_retval = 1;
    receiver_task(&parameter_block);

    // Receiver task will create 4 messages
    // pin1 pos, pin1 both for positive, pin1 neg, pin1 both for negative
    TEST_ASSERT_EQUAL(4, _xQueueSend_buff_idx);

    // Pin1 Pos receives 1
    emk_message_t msg1_converted = {
        .address = *EMK_GROUP_COMMAND_ADDR(1, 1),
        .data = (emk_data_t) {
            .type = DATA_TYPE_B8,
            .of.b8 = 1
        }
    };
    TEST_ASSERT_EQUAL_MSG(msg1_converted, _xQueueSend_buff[0]);

    // Pin1 Both receives 1
    emk_message_t msg2_converted = {
        .address = *EMK_GROUP_COMMAND_ADDR(3, 1),
        .data = (emk_data_t) {
            .type = DATA_TYPE_B8,
            .of.b8 = 1
        }
    };
    TEST_ASSERT_EQUAL_MSG(msg2_converted, _xQueueSend_buff[1]);

    // Pin1 neg receives 0
    emk_message_t msg3_converted = {
        .address = *EMK_GROUP_COMMAND_ADDR(2, 1),
        .data = (emk_data_t) {
            .type = DATA_TYPE_B8,
            .of.b8 = 0
        }
    };
    TEST_ASSERT_EQUAL_MSG(msg3_converted, _xQueueSend_buff[2]);

    // Pin1 both receives 0
    emk_message_t msg4_converted = {
        .address = *EMK_GROUP_COMMAND_ADDR(3, 1),
        .data = (emk_data_t) {
            .type = DATA_TYPE_B8,
            .of.b8 = 0
        }
    };
    TEST_ASSERT_EQUAL_MSG(msg4_converted, *_xQueueSend_top());
}
