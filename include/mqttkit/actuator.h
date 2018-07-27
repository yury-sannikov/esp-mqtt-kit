#ifndef __ESP_MQTT_KIT_ACTUATOR_H__
#define __ESP_MQTT_KIT_ACTUATOR_H__

#include <stdint.h>
#include "address.h"

struct _emk_actuator;
struct _gpio_actuator_configuration;
typedef struct _emk_actuator emk_actuator_t;
typedef struct _gpio_actuator_configuration emk_gpio_actuator_configuration;

typedef enum _emk_actuator_type {
    ACTUATOR_TYPE_GPIO = 1,
} emk_actuator_type_t;

struct _gpio_actuator_configuration {
    // GPIO port number
    uint8_t gpio;
};

#define GPIO_ACTUATOR_CFG(...) \
    &(emk_gpio_actuator_configuration) { __VA_ARGS__ }

struct _emk_actuator {
    // Actuator name, debugging only
    const char* name;
    // Actuator type
    emk_actuator_type_t type;
    // Address which will be used for actuator
    const emk_address_t* address;
    // Actuator specific configuration
    const void* config;
};

#define GPIO_ACTUATOR(aName, ...) \
    &(emk_actuator_t) { \
        .name=aName, \
        .type=ACTUATOR_TYPE_GPIO, \
        ##__VA_ARGS__ \
    }

#endif // __ESP_MQTT_KIT_ACTUATOR_H__
