#ifndef __ESP_MQTT_KIT_DEBUG_H__
#define __ESP_MQTT_KIT_DEBUG_H__

void __emq_abort(const char* message, ...);
void __emq_warning(const char* message, ...);

#define ABORT(message, ...) __emq_abort(message, ##__VA_ARGS__);
#define WARNING(message, ...) __emq_warning(message, ##__VA_ARGS__);

#endif //__ESP_MQTT_KIT_DEBUG_H__
