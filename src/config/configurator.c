#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mqttkit/mqttkit.h"
#include "mqttkit/debug.h"
#include "mqttkit/driver.h"
#include "mqttkit/message.h"

void emk_initialize(const emk_config_t* cfg) {
    if (!cfg) {
        ABORT("emk_add_gpio_ingestors has null emk_config_t supplied");
        return;
    }
  RETAINED_PTR emk_gpio_irq_block_t* iblock = _create_gpio_irq_block(cfg);

  QueueHandle_t message_queue = xQueueCreate(MAX_MESSAGE_QUEUE_SIZE, sizeof(emk_message_t));
  iblock->queue = message_queue;

  _register_interrupt_handlers(iblock);

}

IRAM_DATA emk_gpio_irq_block_t  __gpio_irq_block;

// Go over registered drivers and invoke gpio_iqr_block with `__gpio_irq_block` variable.
// Return pointer to the `__gpio_irq_block` with all fields set up
RETAINED_PTR emk_gpio_irq_block_t*  _create_gpio_irq_block(const emk_config_t* cfg) {
    if (!cfg) {
        ABORT("_create_gpio_irq_block has null emk_config_t supplied");
        return NULL;
    }

    memset(&__gpio_irq_block, 0, sizeof(emk_gpio_irq_block_t));

    uint16_t used_pins = cfg->reserved_pins;

    for (const emk_group_t **group_it = cfg->groups; *group_it; group_it++) {
        const emk_group_t *group = *group_it;
        for (const emk_ingestor_t **ingestor_it = group->ingestors; *ingestor_it; ingestor_it++) {
            const emk_ingestor_t *ingestor = *ingestor_it;

            const emk_driver_t *driver = emk_get_driver(DRIVER_TYPE_INGESTOR, ingestor->type);

            if (!driver) {
                ABORT("Unable to find driver %04x:%04X\n", (int)DRIVER_TYPE_INGESTOR, (int)ingestor->type);
                return NULL;
            }
            // GPIO ingestor can effectively reuse pins so supply only reserved_pins as an input
            uint16_t pins = cfg->reserved_pins;
            driver->init_gpio(&pins, ingestor, group, cfg);
            // Collect used pins for actuators
            used_pins |= pins;

            driver->gpio_iqr_block(&__gpio_irq_block, ingestor);
        }
        for (const emk_actuator_t **actuator_it = group->actuators; *actuator_it; actuator_it++) {
            const emk_actuator_t *actuator = *actuator_it;

            const emk_driver_t *driver = emk_get_driver(DRIVER_TYPE_ACTUATOR, actuator->type);

            if (!driver) {
                ABORT("Unable to find driver %04x:%04X\n", (int)DRIVER_TYPE_ACTUATOR, (int)actuator->type);
                return NULL;
            }
            // Actuators can not reuse pins
            driver->init_gpio(&used_pins, actuator, group, cfg);
        }

    }

    return &__gpio_irq_block;
}

void _register_interrupt_handlers(const RETAINED_PTR emk_gpio_irq_block_t* block) {
    uint16_t pins = block->active_pins;
    uint8_t gpio_idx;
    while ((gpio_idx = __builtin_ffs(pins)))
    {
        gpio_idx--;
        pins &= ~BIT(gpio_idx);
        gpio_set_interrupt(gpio_idx, GPIO_INTTYPE_EDGE_ANY, gpio_ingestor_interrupt_handler);
    }
}