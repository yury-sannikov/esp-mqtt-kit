#include <string.h>
#include "unity.h"
#include "helpers/common_types.h"
#include "mqttkit/mqttkit.h"

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
    return 1;
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
emk_message_t _xQueueSendFromISR_msg_buff[10];
int _xQueueSendFromISR_msg_buff_idx = 0;
BaseType_t* _xQueueSendFromISR_pxHigherPriorityTaskWoken;
BaseType_t xQueueSendFromISR(QueueHandle_t xQueue, const void *pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken) {
    _xQueueSendFromISR_xQueue = xQueue;
    _xQueueSendFromISR_pxHigherPriorityTaskWoken = pxHigherPriorityTaskWoken;
    memcpy(&_xQueueSendFromISR_msg, pvItemToQueue, sizeof(emk_message_t));
    memcpy(&_xQueueSendFromISR_msg_buff[_xQueueSendFromISR_msg_buff_idx++], pvItemToQueue, sizeof(emk_message_t));
    if (_xQueueSendFromISR_msg_buff_idx >= 10) {
        _xQueueSendFromISR_msg_buff_idx = 0;
    }
    return pdTRUE;
}

void _xQueueSendFromISR_clear() {
    _xQueueSendFromISR_xQueue = 0;
    memset(&_xQueueSendFromISR_msg, 0, sizeof(emk_message_t));
    _xQueueSendFromISR_pxHigherPriorityTaskWoken = NULL;
    memset(&_xQueueSendFromISR_msg_buff, 0, sizeof(emk_message_t) * 10);
    _xQueueSendFromISR_msg_buff_idx = 0;
}


const emk_message_t* _xQueueReceive_pvBuffer;
BaseType_t _xQueueReceive_retval;
BaseType_t xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait) {
    (void)xQueue;
    (void)xTicksToWait;
    memcpy(pvBuffer, _xQueueReceive_pvBuffer, sizeof(emk_message_t));
    return _xQueueReceive_retval;
}

uint8_t _gpio_write__gpio_num;
bool _gpio_write__set;
void gpio_write(const uint8_t gpio_num, const bool set) {
    _gpio_write__gpio_num = gpio_num;
    _gpio_write__set = set;
}

void _gpio_write_clear() {
    _gpio_write__gpio_num = 0;
    _gpio_write__set = false;
}


emk_message_t _xQueueSend_buff[xQueueSend_SIZE];
int _xQueueSend_buff_idx  = 0;
BaseType_t xQueueSend(QueueHandle_t xQueue, const void *pvItemToQueue, TickType_t xTicksToWait) {
    (void)xTicksToWait;
    (void)xQueue;
    memcpy(&_xQueueSend_buff[_xQueueSend_buff_idx++], pvItemToQueue, sizeof(emk_message_t));
    if (_xQueueSend_buff_idx >= xQueueSend_SIZE) {
        _xQueueSend_buff_idx = 0;
    }
    return pdTRUE;

}
emk_message_t* _xQueueSend_top() {
    return &_xQueueSend_buff[_xQueueSend_buff_idx == 0 ? xQueueSend_SIZE - 1 : _xQueueSend_buff_idx - 1];
}

bool _xQueueSend_unshift(emk_message_t* dst_message) {
    if (_xQueueSend_buff_idx == 0) {
        return false;
    }
    --_xQueueSend_buff_idx;
    // Copy result from the earlies message available
    memcpy(dst_message, &_xQueueSend_buff[0], sizeof(emk_message_t));
    // Return if nothing left to move
    if (_xQueueSend_buff_idx == 0) {
        return true;
    }
    // Shift messages. Overlapping is fine here
    memcpy(&_xQueueSend_buff[0], &_xQueueSend_buff[1], sizeof(emk_message_t) * _xQueueSend_buff_idx);
    return true;
}

void _xQueueSend_clear(void) {
    memset(&_xQueueSend_buff, 0, sizeof(emk_message_t) * xQueueSend_SIZE);
    _xQueueSend_buff_idx  = 0;
}


uint8_t _gpio_enable_gpio_num;
gpio_direction_t _gpio_enable_direction;
void gpio_enable(const uint8_t gpio_num, const gpio_direction_t direction) {
    _gpio_enable_gpio_num = gpio_num;
    _gpio_enable_direction = direction;
}
void _gpio_enable_clear() {
    _gpio_enable_gpio_num = 0;
    _gpio_enable_direction = GPIO_NOT_SET;
}


void test_send_message(const emk_message_t* msg, void* parameter_block) {
    _xQueueReceive_pvBuffer = msg;
    _xQueueReceive_retval = 1;
    _task_tick_count += 100;
    receiver_task((emk_task_parameter_block_t*)parameter_block);
}

bool test_process_next(void* parameter_block) {
    emk_message_t msg;
    if (!_xQueueSend_unshift(&msg)) {
        return false;
    }
    _xQueueReceive_pvBuffer = &msg;
    _xQueueReceive_retval = 1;
    _task_tick_count += 100;
    receiver_task((emk_task_parameter_block_t*)parameter_block);
    return true;
}
