#ifndef __ESP_MQTT_KIT_DATATYPES_H__
#define __ESP_MQTT_KIT_DATATYPES_H__
#include <stdint.h>
#include <stdio.h>
#include "mqttkit/address.h"

struct _emk_data;
typedef struct _emk_data emk_data_t;

typedef enum {
    DATA_TYPE_INVALID = 0,
    DATA_TYPE_GPIO = 1,
    DATA_TYPE_B8 = 2
} emk_data_type_t;


typedef struct {
    uint8_t gpio_num : 4;
    uint8_t gpio_val : 4;
} emk_gpio_data_t;

struct _emk_data {
    // data type discriminator
    uint8_t type;
    union {
        emk_gpio_data_t gpio;
        uint8_t b8;
    } of;
};

#define DEBUG_DATA(theMsg, theData) \
    DEBUG_NL("%s", (theMsg)); \
    switch((theData).type) { \
        case DATA_TYPE_INVALID: printf("`Invalid`\n"); break; \
        case DATA_TYPE_GPIO: printf("GPIO=%d, VAL=%d\n", (theData).of.gpio.gpio_num, (theData).of.gpio.gpio_val); break; \
        case DATA_TYPE_B8: printf("BYTE8=%d\n", (theData).of.b8); break; \
        default: printf("TODO: Add type to the switch %d\n", (theData).type); break; \
    }

#endif //__ESP_MQTT_KIT_DATATYPES_H__