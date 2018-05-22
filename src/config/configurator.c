#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mqttkit/mqttkit.h"
#include "mqttkit/debug.h"
#include "mqttkit/driver.h"

void emk_initialize(const emk_config_t* cfg) {
    if (!cfg) {
        ABORT("emk_add_gpio_ingestors has null emk_config_t supplied");
        return;
    }
    // uint32_t gpioMask = 0;
    // for (emk_ingestor_t **ingestor_it = ingestors; *ingestor_it; ingestor_it++) {
    //     emk_ingestor_t *ingestor = *ingestor_it;
    //     printf("add %s\n", ingestor->name);
    //     // uint32_t bitNumber = 1 << ingestor->gpio;
    //     // if (gpioMask & bitNumber) {
    //     //     ABORT("emk_add_gpio_ingestors has reused GPIO %d for %s", ingestor->gpio, ingestor->name);
    //     // }
    // }

}

static IRAM_DATA emk_gpio_irq_block_t  __gpio_irq_block;

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
            driver->check_gpio(&used_pins, ingestor, group, cfg);
            driver->gpio_iqr_block(&__gpio_irq_block, ingestor);
        }
    }

    return &__gpio_irq_block;
}
