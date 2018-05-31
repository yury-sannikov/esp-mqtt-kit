#include <string.h>
#include "unity.h"
#include "unity_fixture.h"
#include "helpers/common_types.h"
#include "mqttkit/mqttkit.h"
#include "helpers/testing.h"
#include "mqttkit/driver.h"

TEST_GROUP_RUNNER(GPIO_ISR)
{
  RUN_TEST_CASE(GPIO_ISR, return_if_inactive);
  RUN_TEST_CASE(GPIO_ISR, return_if_out_of_bound);
  RUN_TEST_CASE(GPIO_ISR, debounce_no_overlap);
  RUN_TEST_CASE(GPIO_ISR, debounce_with_overlap);
  RUN_TEST_CASE(GPIO_ISR, debounce_with_overlap2);
  RUN_TEST_CASE(GPIO_ISR, pass_no_overlap);
  RUN_TEST_CASE(GPIO_ISR, pass_with_overlap);
  RUN_TEST_CASE(GPIO_ISR, pass_with_default_time);
  RUN_TEST_CASE(GPIO_ISR, high_pin_triggering);
  RUN_TEST_CASE(GPIO_ISR, low_pin_triggering);
  RUN_TEST_CASE(GPIO_ISR, anyedge_pin_triggering);
  RUN_TEST_CASE(GPIO_ISR, send_status_test);
}

TEST_GROUP(GPIO_ISR);
TEST_SETUP(GPIO_ISR)
{
  clearAbort();
  _gpio_isr_status = 0;
  memset(&__gpio_irq_block, 0, sizeof(emk_gpio_irq_block_t));
  _xQueueSendFromISR_clear();
}

TEST_TEAR_DOWN(GPIO_ISR)
{
}

TEST(GPIO_ISR, return_if_inactive) {
    gpio_ingestor_interrupt_handler(0);
    TEST_ASSERT_EQUAL(-1, _gpio_isr_status);
}

TEST(GPIO_ISR, return_if_out_of_bound) {
    __gpio_irq_block.active_pins = 0xFFFF;
    gpio_ingestor_interrupt_handler(16);
    TEST_ASSERT_EQUAL(-1, _gpio_isr_status);
}
TEST(GPIO_ISR, debounce_no_overlap) {
    __gpio_irq_block.active_pins = 0xFFFF;
    __gpio_irq_block.last_irq[0] = 100;
    __gpio_irq_block.debouce_values[0] = 50;
    _task_tick_count = 120;
    gpio_ingestor_interrupt_handler(0);
    TEST_ASSERT_EQUAL(-4, _gpio_isr_status);
}

TEST(GPIO_ISR, debounce_with_overlap) {
    // Exactly 50 ticks
    __gpio_irq_block.active_pins = 0xFFFF;
    __gpio_irq_block.last_irq[0] = ~((TickType_t) 0);
    __gpio_irq_block.debouce_values[0] = 50;
    _task_tick_count = 49; // One tick to make from 0xFF.. to 0 + 49 should give 50
    gpio_ingestor_interrupt_handler(0);
    TEST_ASSERT_EQUAL(-4, _gpio_isr_status);
}

TEST(GPIO_ISR, debounce_with_overlap2) {
    __gpio_irq_block.active_pins = 0xFFFF;
    __gpio_irq_block.last_irq[0] = (~((TickType_t) 0)) - 20;
    __gpio_irq_block.debouce_values[0] = 50;
    _task_tick_count = 29;
    gpio_ingestor_interrupt_handler(0);
    TEST_ASSERT_EQUAL(-4, _gpio_isr_status);
    TEST_ASSERT_FALSE(_task_tick_count == __gpio_irq_block.last_irq[0]);
}

TEST(GPIO_ISR, pass_no_overlap) {
    __gpio_irq_block.active_pins = 0xFFFF;
    __gpio_irq_block.last_irq[0] = 100;
    __gpio_irq_block.debouce_values[0] = 50;
    _task_tick_count = 151;
    gpio_ingestor_interrupt_handler(0);
    TEST_ASSERT_EQUAL(0, _gpio_isr_status);
    TEST_ASSERT_EQUAL(_task_tick_count, __gpio_irq_block.last_irq[0]);
}

TEST(GPIO_ISR, pass_with_overlap) {
    __gpio_irq_block.active_pins = 0xFFFF;
    __gpio_irq_block.last_irq[0] = ~((TickType_t) 0);
    __gpio_irq_block.debouce_values[0] = 50;
    _task_tick_count = 50;
    gpio_ingestor_interrupt_handler(0);
    TEST_ASSERT_EQUAL(0, _gpio_isr_status);

    __gpio_irq_block.active_pins = 0xFFFF;
    __gpio_irq_block.last_irq[0] = ~((TickType_t) 0) - 15;
    __gpio_irq_block.debouce_values[0] = 50;
    _task_tick_count = 35;
    gpio_ingestor_interrupt_handler(0);
    TEST_ASSERT_EQUAL(0, _gpio_isr_status);
    TEST_ASSERT_EQUAL(_task_tick_count, __gpio_irq_block.last_irq[0]);
}

TEST(GPIO_ISR, pass_with_default_time) {
    __gpio_irq_block.active_pins = 0xFFFF;
    __gpio_irq_block.last_irq[0] = ~((TickType_t) 0);
    __gpio_irq_block.debouce_values[0] = 50;
    _task_tick_count = IRQ_LAST_TRIGGERED_DEFAULT_TIME;
    gpio_ingestor_interrupt_handler(0);
    TEST_ASSERT_EQUAL(0, _gpio_isr_status);
    TEST_ASSERT_EQUAL(IRQ_LAST_TRIGGERED_DEFAULT_TIME + 1, __gpio_irq_block.last_irq[0]);
}

TEST(GPIO_ISR, high_pin_triggering) {
    __gpio_irq_block.active_pins = 0xFFFF;
    __gpio_irq_block.last_irq[0] = 0;
    __gpio_irq_block.debouce_values[0] = 50;
    _task_tick_count = 100;
    _gpio_read_value = true;

    // No triggering if pos_edge bit is not set
    gpio_ingestor_interrupt_handler(0);
    TEST_ASSERT_EQUAL(0, _gpio_isr_status);
    __gpio_irq_block.pos_edge = 0x1;
    _task_tick_count = 151;
    gpio_ingestor_interrupt_handler(0);
    TEST_ASSERT_EQUAL(1, _gpio_isr_status);
}

TEST(GPIO_ISR, low_pin_triggering) {
    __gpio_irq_block.active_pins = 0xFFFF;
    __gpio_irq_block.last_irq[0] = 0;
    __gpio_irq_block.debouce_values[0] = 50;
    _task_tick_count = 100;
    _gpio_read_value = false;

    // No triggering if neg_edge bit is not set
    gpio_ingestor_interrupt_handler(0);
    TEST_ASSERT_EQUAL(0, _gpio_isr_status);
    __gpio_irq_block.neg_edge = 0x1;
    _task_tick_count = 151;
    gpio_ingestor_interrupt_handler(0);
    TEST_ASSERT_EQUAL(2, _gpio_isr_status);

}

TEST(GPIO_ISR, anyedge_pin_triggering) {
    __gpio_irq_block.active_pins = 0xFFFF;
    __gpio_irq_block.last_irq[0] = 0;
    __gpio_irq_block.debouce_values[0] = 50;
    _task_tick_count = 100;
    _gpio_read_value = false;

    __gpio_irq_block.pos_edge = 0x1;
    __gpio_irq_block.neg_edge = 0x1;
    gpio_ingestor_interrupt_handler(0);
    TEST_ASSERT_EQUAL(2, _gpio_isr_status);

    _gpio_read_value = true;
    _task_tick_count = 151;
    gpio_ingestor_interrupt_handler(0);
    TEST_ASSERT_EQUAL(1, _gpio_isr_status);
}

TEST(GPIO_ISR, send_status_test) {
    __gpio_irq_block.active_pins = 0xFFFF;
    __gpio_irq_block.last_irq[3] = 0;
    __gpio_irq_block.debouce_values[3] = 50;
    _task_tick_count = 100;
    _gpio_read_value = true;
    __gpio_irq_block.pos_edge = 0x8;
    gpio_ingestor_interrupt_handler(3);
    TEST_ASSERT_EQUAL(1, _gpio_isr_status);

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

    TEST_ASSERT_EQUAL_MEMORY(&msg, &_xQueueSendFromISR_msg, sizeof(emk_message_t));
}
