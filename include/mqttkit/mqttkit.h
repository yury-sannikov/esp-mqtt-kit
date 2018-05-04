#ifndef __ESP_MQTT_KIT_H__
#define __ESP_MQTT_KIT_H__

#include "mqttkit/configurator.h"

// Create configuration instance
emk_config_t* emk_create_configuration(void);

void mqtt_kit_init(emk_config_t *config);

#endif // __ESP_MQTT_KIT_H__