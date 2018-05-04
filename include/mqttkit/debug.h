#ifndef __ESP_MQTT_KIT_DEBUG_H__
#define __ESP_MQTT_KIT_DEBUG_H__

void __emq_abort(const char* message, ...);
#define ABORT(message, ...) __emq_abort(message, ##__VA_ARGS__);

#endif //__ESP_MQTT_KIT_DEBUG_H__
