#ifndef __ESP_MQTT_KIT_H__
#define __ESP_MQTT_KIT_H__


typedef struct {
    char *test;
} mqtt_kit_config_t;

void mqtt_kit_init(mqtt_kit_config_t *config);

#endif // __ESP_MQTT_KIT_H__