#ifndef __ESP_MQTT_KIT_ACTUATOR_H__
#define __ESP_MQTT_KIT_ACTUATOR_H__

#include <stdint.h>
#include "address.h"
#include "typedefs.h"

struct _emk_actuator;
struct _gpio_actuator_configuration;

typedef enum _emk_actuator_type {
    ACTUATOR_TYPE_GPIO = 1,
} emk_actuator_type_t;

struct _gpio_actuator_configuration {
    // GPIO port number
    uint8_t gpio;
    // Initial GPIO state
    uint8_t initial_state;
    // A mask to be applied to the gpio_read() result
    uint8_t safety_mask;
    // gpio_read() & safety_mask should match safety_value in order to set new GPIO value
    uint8_t safety_value;
};

// A value used for initial_status_timeout to indicate initial notification status disable
#define DISABLE_INITIAL_NOTIFICATION_TIMEOUT ((uint32_t)-1)
// Set this flag along with GPIO value to disable successfull feedback message
#define GPIO_ACTUATOR_NOFEEDBACK 0x80
// A mask to be used to get a value from the message
#define GPIO_ACTUATOR_VALUE_MASK 0x01

#define GPIO_ACTUATOR_CFG(...) \
    &(emk_gpio_actuator_configuration) { __VA_ARGS__ }

struct _emk_actuator {
    // Actuator name, debugging only
    const char* name;
    // Actuator type
    emk_actuator_type_t type;
    // Address which will be used for actuator
    const emk_address_t* address;
    // Address which will be used to send status updates
    const emk_address_t* status_address;
    // Timeout of initial status advertizing. Use DISABLE_INITIAL_NOTIFICATION_TIMEOUT to disable initial notification
    uint32_t initial_status_timeout;
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
