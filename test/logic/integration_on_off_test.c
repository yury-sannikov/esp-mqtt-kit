#include <string.h>
#include "unity.h"
#include "unity_fixture.h"
#include "helpers/common_types.h"
#include "mqttkit/mqttkit.h"
#include "helpers/testing.h"
#include "mqttkit/driver.h"
#include "mqttkit/logic.h"
#include "mqttkit/debug.h"

TEST_GROUP_RUNNER(INTEGRATION_ON_OFF)
{
  RUN_TEST_CASE(INTEGRATION_ON_OFF, integration_on_off_test);
}

emk_task_parameter_block_t parameter_block;

TEST_GROUP(INTEGRATION_ON_OFF);
TEST_SETUP(INTEGRATION_ON_OFF)
{
    clearAbort();
    _xQueueSend_clear();
    __gpio_irq_block.queue = 1;
    _task_tick_count = 0;
    parameter_block.irq_block = &__gpio_irq_block;
}

TEST_TEAR_DOWN(INTEGRATION_ON_OFF)
{
}

/* This test create 2 buttons. Firs one works by positive edge and send C/1 message with the status
   Second button works by negative edge and send C/1 message as well
   Actuator set up on GPIO=3 and use the same C/1 addres.
Behavior:
    Turn led ON when buton 1 clicked. Turn led off, when button 2 clicked (released)
*/
TEST(INTEGRATION_ON_OFF, integration_on_off_test) {
    const emk_ingestor_t *ingestor_data[] = {
        GPIO_INGESTOR("button ON",
            .address=EMK_COMMAND_ADDR(1),
            .config=GPIO_INGESTOR_CFG(
                .gpio = 1,
                .edge = EMK_GPIO_EDGE_POS,
                .debounce = 50
            )
        ),
        GPIO_INGESTOR("button OFF",
            .address=EMK_COMMAND_ADDR(1),
            .config=GPIO_INGESTOR_CFG(
                .gpio = 2,
                .edge = EMK_GPIO_EDGE_NEG,
                .debounce = 50
            )
        ),
        NULL
    };

    const emk_actuator_t *actuator_data[] = {
        GPIO_ACTUATOR("Controlled LED",
            .address=EMK_COMMAND_ADDR(1),
            .config=GPIO_ACTUATOR_CFG(
                .gpio = 3
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
        .groups = groups,
        .reserved_pins = 0
    };
    parameter_block.config = &config;

    _gpio_write_clear();
    _create_gpio_irq_block(&config);
    __gpio_irq_block.queue = 1;


    // Press button ON
    test_send_message(&(emk_message_t){
        .address = EMK_SYS_MIDDLEWARE_ADDR(DRIVER_TYPE_INGESTOR),
        .data = (emk_data_t) {
            .type = DATA_TYPE_GPIO,
            .of.gpio = (emk_gpio_data_t) {
                .gpio_num = 1,
                .gpio_val = 1
            }
        }
    }, &parameter_block);
    // Press button OFF
    test_send_message(&(emk_message_t){
        .address = EMK_SYS_MIDDLEWARE_ADDR(DRIVER_TYPE_INGESTOR),
        .data = (emk_data_t) {
            .type = DATA_TYPE_GPIO,
            .of.gpio = (emk_gpio_data_t) {
                .gpio_num = 2,
                .gpio_val = 0
            }
        }
    }, &parameter_block);
    // Ingestor middleware should generate 2 messages
    TEST_ASSERT_EQUAL(2, _xQueueSend_buff_idx);

    // `Controlled LED` ON
    _gpio_write_clear();
    test_process_next(&parameter_block);
    TEST_ASSERT_EQUAL(3, _gpio_write__gpio_num);
    TEST_ASSERT_EQUAL(1, _gpio_write__set);

    // `Controlled LED` OFF
    _gpio_write_clear();
    test_process_next(&parameter_block);
    TEST_ASSERT_EQUAL(3, _gpio_write__gpio_num);
    TEST_ASSERT_EQUAL(0, _gpio_write__set);

}