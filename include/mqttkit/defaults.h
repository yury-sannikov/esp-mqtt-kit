#ifndef __ESP_MQTT_KIT_DEFAULTS_H__
#define __ESP_MQTT_KIT_DEFAULTS_H__

// RTOS Queue size for Logic thread
#define MAX_MESSAGE_QUEUE_SIZE 20

// GPIO ingestor default debounce interval in msec. Set to zero if you're use
// proper RC/invertor debouncer and sure that MCU will not trigger a lot of interrupts.
// If you use RC-only debouncer, please use default debounce value. RC filter usage will result
// to some of the false interrupts being generated.
#define DEFAULT_DEBOUNCE_MSEC 50


#endif // __ESP_MQTT_KIT_DEFAULTS_H__
