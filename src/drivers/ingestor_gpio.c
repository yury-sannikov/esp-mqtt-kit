#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "mqttkit/driver.h"
#include "mqttkit/ingestor.h"
#include "mqttkit/debug.h"

void ingestor_gpio__check_gpio(uint16_t* used_pins, const emk_ingestor_t* ingestor, const emk_group_t *group, const emk_config_t* config) {
    (void)config;
    const emk_gpio_ingestor_configuration* cfg = (const emk_gpio_ingestor_configuration*)ingestor->config;
    uint16_t pin_m = BIT(cfg->gpio);
    if (*used_pins & pin_m) {
        ABORT("Ingestor %s in group %s attempt to reuse pin %d", ingestor->name, group->name, cfg->gpio);
        return;
    }
}

void ingestor_gpio__gpio_iqr_block(emk_gpio_irq_block_t* gpio_irq_block, const emk_ingestor_t* ingestor) {
    (void)gpio_irq_block;
    (void)ingestor;
    printf("ingestor_gpio__gpio_iqr_block~\n");
}
