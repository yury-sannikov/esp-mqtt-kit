#ifndef __ESP_MQTT_KIT_DATATYPES_H__
#define __ESP_MQTT_KIT_DATATYPES_H__
#include <stdint.h>
#include "mqttkit/address.h"

struct _emk_data;
typedef struct _emk_data emk_data_t;

typedef enum {
    DATA_TYPE_INVALID = 0,
    DATA_TYPE_GPIO = 1
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
    } of;
};


#endif //__ESP_MQTT_KIT_DATATYPES_H__