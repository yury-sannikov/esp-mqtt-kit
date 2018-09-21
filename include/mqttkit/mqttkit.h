#ifndef __ESP_MQTT_KIT_H__
#define __ESP_MQTT_KIT_H__

#include "mqttkit/typedefs.h"
#include "mqttkit/configurator.h"
#include "mqttkit/actuator.h"
#include "mqttkit/ingestor.h"
#include "mqttkit/logic.h"

// Initialize MQTT kit with configuration
void mqtt_kit_init(const emk_config_t *config);

// Message receiver task
int receiver_task( void *pvParameters );

// Process message against logic blocks and put resulting message into the message queue
void emk_process_logic(const emk_config_t* config, const emk_message_t *message, emk_context_t* context);

// Create emk_gpio_irq_block_t from configuration or abort() if error detected.
RETAINED_PTR emk_gpio_irq_block_t*  _create_gpio_irq_block(const emk_config_t* cfg);

// register interrupt handlers according to the emk_gpio_irq_block_t values
void _register_interrupt_handlers(const RETAINED_PTR emk_gpio_irq_block_t* block);

void _init_logic_blocks(const emk_config_t* cfg);

void emk_initialize(const emk_config_t* cfg);

#endif // __ESP_MQTT_KIT_H__