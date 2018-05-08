#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "mqttkit/driver.h"
#include "mqttkit/ingestor.h"


void ingestor_gpio__check_gpio(uint16_t* used_pins, const emk_ingestor_t* ingestor) {
    (void)used_pins;
    (void)ingestor;
    printf("ingestor_gpio__check_gpio~\n");
}

void ingestor_gpio__gpio_iqr_block(emk_gpio_irq_block_t* gpio_irq_block, const emk_ingestor_t* ingestor) {
    (void)gpio_irq_block;
    (void)ingestor;
    printf("ingestor_gpio__gpio_iqr_block~\n");
}


// Check pin usage
// switch(ingestor->type) {
//     case INGESTOR_TYPE_GPIO: {
//         const emk_gpio_ingestor_configuration* cfg = (const emk_gpio_ingestor_configuration*)ingestor->config;
//         uint32_t pin_m = BIT(cfg->gpio);
//         if (used_pins & pin_m) {
//             ABORT("Ingestor %s in group %s attempt to use", ingestor->name, group->name);
//             return NULL;
//         }

//         break;
//     }
//     default:
//         break;
// }
// (void)ingestor;

