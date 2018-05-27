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
    if (pin_m == 0 || cfg->gpio > 15) {
        ABORT("Ingestor %s in group %s attempt to use out of bound pin %d", ingestor->name, group->name, cfg->gpio);
        return;
    }

    if (*used_pins & pin_m) {
        ABORT("Ingestor %s in group %s attempt to reuse pin %d", ingestor->name, group->name, cfg->gpio);
        return;
    }

}

void ingestor_gpio__gpio_iqr_block(emk_gpio_irq_block_t* gpio_irq_block, const emk_ingestor_t* ingestor) {
    const emk_gpio_ingestor_configuration* cfg = (const emk_gpio_ingestor_configuration*)ingestor->config;
    uint16_t pin_m = BIT(cfg->gpio);
    gpio_irq_block->active_pins |= pin_m;
    gpio_irq_block->state &= ~pin_m;
    // set last irq to special value so first IRQ will be triggered anyway
    gpio_irq_block->last_irq[cfg->gpio] = IRQ_LAST_TRIGGERED_DEFAULT_TIME;
    uint32_t debounce = cfg->debounce;
    if (debounce == 0) {
        debounce = DEFAULT_DEBOUNCE_MSEC;
    }
    gpio_irq_block->debouce_values[cfg->gpio] = debounce / portTICK_PERIOD_MS;
}

emk_driver_middleware_result_t ingestor_gpio__message_middleware(const emk_config_t* config, void *message) {
    (void)config;
    (void)message;
    return MIDDLEWARE_RESULT_NOT_HANDLED;
}