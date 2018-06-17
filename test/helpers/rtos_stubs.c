#include <string.h>
#include "unity.h"
#include "helpers/common_types.h"

int __test_gpio_set_interrupt_count = 0;
int __test_gpio_set_interrupt_bits = 0;

void gpio_set_interrupt(const uint8_t gpio_num, const int int_type, gpio_interrupt_handler_t handler) {
    ++__test_gpio_set_interrupt_count;
    __test_gpio_set_interrupt_bits |= (1 << gpio_num);
    (void)int_type;
    (void)handler;
}


void test_clear_gpio_set_interrupt(void) {
    __test_gpio_set_interrupt_count = 0;
    __test_gpio_set_interrupt_bits = 0;
}

void assert_gpio_set_interrupt(int gpio_mask, int call_count) {
    TEST_ASSERT_EQUAL_INT(gpio_mask, __test_gpio_set_interrupt_bits);
    TEST_ASSERT_EQUAL_INT(call_count, __test_gpio_set_interrupt_count);
    test_clear_gpio_set_interrupt();
}

QueueHandle_t xQueueCreate(UBaseType_t uxQueueLength, UBaseType_t uxItemSize) {
    (void)uxQueueLength;
    (void)uxItemSize;
    return 0;
}

TickType_t _task_tick_count = 0;
TickType_t xTaskGetTickCountFromISR( void ) {
    return _task_tick_count;
}
TickType_t xTaskGetTickCount( void ) {
    return _task_tick_count;
}

bool _gpio_read_value;
bool gpio_read(const uint8_t gpio_num) {
    (void)gpio_num;
    return _gpio_read_value;
}

int _gpio_isr_status = 0;

void _set_gpio_isr_status(int status) {
    _gpio_isr_status = status;
}

QueueHandle_t _xQueueSendFromISR_xQueue;
emk_message_t _xQueueSendFromISR_msg;
BaseType_t* _xQueueSendFromISR_pxHigherPriorityTaskWoken;
BaseType_t xQueueSendFromISR(QueueHandle_t xQueue, const void *pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken) {
    _xQueueSendFromISR_xQueue = xQueue;
    _xQueueSendFromISR_pxHigherPriorityTaskWoken = pxHigherPriorityTaskWoken;
    memcpy(&_xQueueSendFromISR_msg, pvItemToQueue, sizeof(emk_message_t));
    return pdTRUE;
}

void _xQueueSendFromISR_clear() {
    _xQueueSendFromISR_xQueue = 0;
    memset(&_xQueueSendFromISR_msg, 0, sizeof(emk_message_t));
    _xQueueSendFromISR_pxHigherPriorityTaskWoken = NULL;
}


const emk_message_t* _xQueueReceive_pvBuffer;
BaseType_t _xQueueReceive_retval;
BaseType_t xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait) {
    (void)xQueue;
    (void)xTicksToWait;
    memcpy(pvBuffer, _xQueueReceive_pvBuffer, sizeof(emk_message_t));
    return _xQueueReceive_retval;
}
