#ifndef __ESP_MQTT_KIT_MESSAGE_H__
#define __ESP_MQTT_KIT_MESSAGE_H__
#include <stdint.h>

struct _emk_message;
typedef struct _emk_message emk_message_t;

struct _emk_message {
    uint32_t v;
};


#endif //__ESP_MQTT_KIT_MESSAGE_H__