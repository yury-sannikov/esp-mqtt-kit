#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "mqttkit/driver.h"
#include "mqttkit/actuator.h"
#include "mqttkit/debug.h"
#include "mqttkit/context.h"
#include "helpers/common_types.h"

void gpio_actuator__check_gpio(uint16_t* used_pins, const void* driver, const emk_group_t *group, const emk_config_t* config) {
    (void)config;

    const emk_actuator_t* actuator = (const emk_actuator_t*)driver;
    const emk_gpio_actuator_configuration* cfg = (const emk_gpio_actuator_configuration*)actuator->config;

    uint16_t pin_m = BIT(cfg->gpio);
    if (pin_m == 0 || cfg->gpio > 15) {
        ABORT("Actuator %s in group %s attempt to use out of bound pin %d", actuator->name, group->name, cfg->gpio);
        return;
    }

    if (*used_pins & pin_m) {
        ABORT("Actuator %s in group %s attempt to reuse pin %d", actuator->name, group->name, cfg->gpio);
        return;
    }
    *used_pins |= pin_m;

}

void gpio_actuator__gpio_iqr_block(emk_gpio_irq_block_t* irq_block, const void* driver) {
    (void)irq_block;
    (void)driver;
    ABORT("not supported");
}

emk_driver_middleware_result_t gpio_actuator__message_middleware(const emk_config_t* config, const emk_message_t* message, emk_context_t* context) {
    (void)config;
    (void)context;

    for (const emk_group_t **group_it = config->groups; *group_it; group_it++) {
        const emk_group_t *group = *group_it;
        for (const emk_actuator_t **actuator_it = group->actuators; *actuator_it; actuator_it++) {
            const emk_actuator_t *actuator = *actuator_it;
            if (actuator->type != ACTUATOR_TYPE_GPIO) {
                continue;
            }
            if (SAME_ADDRESS(message->address, actuator->address)) {
                continue;
            }
            const emk_gpio_actuator_configuration* cfg = (const emk_gpio_actuator_configuration*)actuator->config;
            uint8_t value = 0;
            switch(message->data.type) {
                case DATA_TYPE_GPIO: value = message->data.of.gpio.gpio_val; break;
                case DATA_TYPE_B8: value = message->data.of.b8; break;
                default: {
                    WARNING("gpio_actuator__message_middleware: Unknown message type %d. Ignored.", message->data.type);
                    continue;
                }
            }

            gpio_write(cfg->gpio, value != 0);
        }
    }

    // Return not handled to allow side effests, such as delivering the same messate to the another logic component
    return MIDDLEWARE_RESULT_NOT_HANDLED;
}