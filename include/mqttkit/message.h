#ifndef __ESP_MQTT_KIT_MESSAGE_H__
#define __ESP_MQTT_KIT_MESSAGE_H__
#include <stdint.h>
#include "mqttkit/address.h"
#include "mqttkit/data_types.h"

struct _emk_message;
typedef struct _emk_message emk_message_t;

struct _emk_message {
    emk_address_t address;
    emk_data_t data;
};



#endif //__ESP_MQTT_KIT_MESSAGE_H__