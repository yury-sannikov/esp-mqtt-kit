#ifndef __ESP_MQTT_KIT__TEST_COMMON_H__
#define __ESP_MQTT_KIT__TEST_COMMON_H__
#include <stdint.h>
#include <stdbool.h>
#include "mqttkit/message.h"

#ifndef BIT
#define BIT(X) (1<<(X))
#endif

// RTOS types
typedef uint32_t TickType_t;
typedef uint32_t QueueHandle_t;
typedef int32_t UBaseType_t;
typedef uint32_t BaseType_t;

// define stuff for tests only
#define IRAM_DATA
#define IRAM
#define portTICK_PERIOD_MS 1
#define GPIO_INTTYPE_EDGE_ANY 0xBAD
#define pdTRUE			( ( BaseType_t ) 1 )
#define pdPASS			( pdTRUE )
#define pdFAIL			( pdFALSE )
#define errQUEUE_EMPTY	( ( BaseType_t ) 0 )
#define errQUEUE_FULL	( ( BaseType_t ) 0 )

typedef enum {
    GPIO_INPUT,
    GPIO_OUTPUT,         /* "Standard" push-pull output */
    GPIO_OUT_OPEN_DRAIN, /* Open drain output */
    GPIO_NOT_SET = 0xFF
} gpio_direction_t;

void gpio_enable(const uint8_t gpio_num, const gpio_direction_t direction);
void _gpio_enable_clear(void);
extern uint8_t _gpio_enable_gpio_num;
extern gpio_direction_t _gpio_enable_direction;

extern int _gpio_isr_status;
void _set_gpio_isr_status(int status);
#define SET_GPIO_ISR_STATUS(status) _set_gpio_isr_status((status));

extern TickType_t _task_tick_count;
TickType_t xTaskGetTickCount( void );
TickType_t xTaskGetTickCountFromISR( void );

typedef void (* gpio_interrupt_handler_t)(uint8_t gpio_num);
void gpio_set_interrupt(const uint8_t gpio_num, const int int_type, gpio_interrupt_handler_t handler);
QueueHandle_t xQueueCreate(UBaseType_t uxQueueLength, UBaseType_t uxItemSize);

extern bool _gpio_read_value;
bool gpio_read(const uint8_t gpio_num);

extern QueueHandle_t _xQueueSendFromISR_xQueue;
extern emk_message_t _xQueueSendFromISR_msg;
extern BaseType_t* _xQueueSendFromISR_pxHigherPriorityTaskWoken;
extern emk_message_t _xQueueSendFromISR_msg_buff[10];
extern int _xQueueSendFromISR_msg_buff_idx;

BaseType_t xQueueSendFromISR(QueueHandle_t xQueue, const void *pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken);
void _xQueueSendFromISR_clear(void);

#define xQueueSend_SIZE 50
extern emk_message_t _xQueueSend_buff[xQueueSend_SIZE];
extern int _xQueueSend_buff_idx;
BaseType_t xQueueSend(QueueHandle_t xQueue, const void *pvItemToQueue, TickType_t xTicksToWait);
void _xQueueSend_clear(void);
bool _xQueueSend_unshift(emk_message_t* dst_message);

extern const emk_message_t* _xQueueReceive_pvBuffer;
extern BaseType_t _xQueueReceive_retval;
BaseType_t xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait);

extern uint8_t _gpio_write__gpio_num;
extern bool _gpio_write__set;
void gpio_write(const uint8_t gpio_num, const bool set);
void _gpio_write_clear(void);

#endif //__ESP_MQTT_KIT__TEST_COMMON_H__
