#ifndef __ESP_MQTT_KIT_CFG_H__
#define __ESP_MQTT_KIT_CFG_H__

#include <stdint.h>
#include "defaults.h"
#include "helpers/common_types.h"
#include "address.h"
#include "group.h"
#include "ingestor.h"

// Returned pointer to a memory should not be freed by user code
#define RETAINED_PTR

struct _emk_config;
struct _emk_gpio_irq_block;
struct _emk_task_parameter_block;

typedef struct _emk_config emk_config_t;
typedef struct _emk_gpio_irq_block emk_gpio_irq_block_t;
typedef struct _emk_task_parameter_block emk_task_parameter_block_t;


struct _emk_config {
    uint16_t reserved_pins;
    const emk_group_t **groups;
};

struct _emk_gpio_irq_block {
    // Message receiving queue
    QueueHandle_t queue;
    // A bit mask of active pins
    uint16_t active_pins;
    // Fire on positive edge
    uint16_t pos_edge;
    // Fire on negative edge
    uint16_t neg_edge;
    // last tick values for the interrupt
    TickType_t last_irq[16];
    // Debounce in ticks
    TickType_t debouce_values[16];
} __attribute__ ((aligned (32)));


struct _emk_task_parameter_block {
    const emk_config_t* config;
    const emk_gpio_irq_block_t* irq_block;
};

// Create emk_gpio_irq_block_t from configuration or abort() if error detected.
RETAINED_PTR emk_gpio_irq_block_t*  _create_gpio_irq_block(const emk_config_t* cfg);

// register interrupt handlers according to the emk_gpio_irq_block_t values
void _register_interrupt_handlers(const RETAINED_PTR emk_gpio_irq_block_t* block);

void emk_initialize(const emk_config_t* cfg);

#endif // __ESP_MQTT_KIT_CFG_H__
