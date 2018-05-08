#ifndef __ESP_MQTT_KIT__TEST_COMMON_H__
#define __ESP_MQTT_KIT__TEST_COMMON_H__
#include <stdint.h>

#ifndef BIT
#define BIT(X) (1<<(X))
#endif

// Emulate TickType_t for RTOS
typedef uint32_t TickType_t;

#define IRAM_DATA

#endif //__ESP_MQTT_KIT__TEST_COMMON_H__
