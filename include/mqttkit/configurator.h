#ifndef __ESP_MQTT_KIT_CFG_H__
#define __ESP_MQTT_KIT_CFG_H__

#include <stdint.h>
#include "helpers/common_types.h"
#include "address.h"
#include "group.h"
#include "ingestor.h"

// Returned pointer to a memory should not be freed by user code
#define RETAINED_PTR

struct _emk_config;
struct _emk_gpio_irq_block;

typedef struct _emk_config emk_config_t;
typedef struct _emk_gpio_irq_block emk_gpio_irq_block_t;


struct _emk_config {
    uint16_t reserved_pins;
    const emk_group_t **groups;
};

struct _emk_gpio_irq_block {
    // A bit mask of active pins
    uint16_t active_pins;
    // Previous triggered state
    uint16_t state;
    // last tick values for the interrupt
    TickType_t last_irq[16];
    // Debounce in ticks
    uint32_t debouce_values[16];
} __attribute__ ((aligned (32)));


// Create emk_gpio_irq_block_t from configuration or abort() if error detected.
RETAINED_PTR emk_gpio_irq_block_t*  _create_gpio_irq_block(const emk_config_t* cfg);


#endif // __ESP_MQTT_KIT_CFG_H__
