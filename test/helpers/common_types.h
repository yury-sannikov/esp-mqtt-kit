#ifndef __ESP_MQTT_KIT__TEST_COMMON_H__
#define __ESP_MQTT_KIT__TEST_COMMON_H__
#include <stdint.h>

#ifndef BIT
#define BIT(X) (1<<(X))
#endif

// RTOS types
typedef uint32_t TickType_t;
typedef uint32_t QueueHandle_t;
typedef uint32_t UBaseType_t;

// define stuff for tests only
#define IRAM_DATA
#define IRAM
#define portTICK_PERIOD_MS 1
#define GPIO_INTTYPE_EDGE_ANY 0xBAD


#define SET_GPIO_ISR_STATUS(status)

TickType_t xTaskGetTickCount( void );
TickType_t xTaskGetTickCountFromISR( void );

typedef void (* gpio_interrupt_handler_t)(uint8_t gpio_num);
void gpio_set_interrupt(const uint8_t gpio_num, const int int_type, gpio_interrupt_handler_t handler);
QueueHandle_t xQueueCreate(UBaseType_t uxQueueLength, UBaseType_t uxItemSize);


#endif //__ESP_MQTT_KIT__TEST_COMMON_H__
