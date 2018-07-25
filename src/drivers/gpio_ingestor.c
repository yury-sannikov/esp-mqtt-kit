#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "mqttkit/driver.h"
#include "mqttkit/ingestor.h"
#include "mqttkit/debug.h"
#include "mqttkit/context.h"
#include "helpers/common_types.h"

void gpio_ingestor__init_gpio(uint16_t* used_pins, const void* driver, const emk_group_t *group, const emk_config_t* config) {
    (void)config;
    const emk_ingestor_t* ingestor = (const emk_ingestor_t*)driver;
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
    *used_pins |= pin_m;
    // Enable this pin for input
    gpio_enable(cfg->gpio, GPIO_INPUT);
}

void gpio_ingestor__gpio_iqr_block(emk_gpio_irq_block_t* gpio_irq_block, const void* driver) {
    const emk_ingestor_t* ingestor = (const emk_ingestor_t*)driver;
    const emk_gpio_ingestor_configuration* cfg = (const emk_gpio_ingestor_configuration*)ingestor->config;
    uint16_t pin_m = BIT(cfg->gpio);
    gpio_irq_block->active_pins |= pin_m;

    if (cfg->edge & EMK_GPIO_EDGE_POS) {
        gpio_irq_block->pos_edge |= pin_m;
    }
    if (cfg->edge & EMK_GPIO_EDGE_NEG) {
        gpio_irq_block->neg_edge |= pin_m;
    }

    // set last irq to special value so first IRQ will be triggered anyway
    gpio_irq_block->last_irq[cfg->gpio] = IRQ_LAST_TRIGGERED_DEFAULT_TIME;
    uint32_t debounce = cfg->debounce;
    if (debounce == 0) {
        debounce = DEFAULT_DEBOUNCE_MSEC;
    }
    gpio_irq_block->debouce_values[cfg->gpio] = debounce / portTICK_PERIOD_MS;
}

emk_driver_middleware_result_t gpio_ingestor__message_middleware(const emk_config_t* config, const emk_message_t* message, emk_context_t* context) {

    // Return if message was not addressed to ingestor
    if (!EMK_IS_SYSTEM_ADDR(message->address) || message->address.s.driver_type != DRIVER_TYPE_INGESTOR) {
        return MIDDLEWARE_RESULT_NOT_HANDLED;
    }

    const emk_gpio_data_t* gpio_data = &message->data.of.gpio;

    for (const emk_group_t **group_it = config->groups; *group_it; group_it++) {
        const emk_group_t *group = *group_it;
        if (group->ingestors == NULL) {
            ABORT("Null ingestors in group %s", group->name);
            return MIDDLEWARE_RESULT_NOT_HANDLED;
        }
        for (const emk_ingestor_t **ingestor_it = group->ingestors; *ingestor_it; ingestor_it++) {
            const emk_ingestor_t *ingestor = *ingestor_it;
            if (ingestor->type != INGESTOR_TYPE_GPIO) {
                continue;
            }

            const emk_gpio_ingestor_configuration* cfg = (const emk_gpio_ingestor_configuration*)ingestor->config;

            // Multiple addresses might be assigned to the same pin
            // Check if this particular configuration should convert GPIO message
            bool should_trigger = ((cfg->edge & EMK_GPIO_EDGE_POS) && (gpio_data->gpio_val != 0)) ||
                ((cfg->edge & EMK_GPIO_EDGE_NEG) && (gpio_data->gpio_val == 0));

            if (cfg->gpio == gpio_data->gpio_num && should_trigger) {
                // Add group to the address, if no group has been specified
                emk_address_t address_with_group;
                EMK_ADDRESS_MERGE_WITH_GROUP(address_with_group, ingestor->address, *group);
                emk_message_t msg = {
                    .address = address_with_group,
                    .data = (emk_data_t) {
                        .type = DATA_TYPE_B8,
                        .of.b8 = gpio_data->gpio_val
                    }
                };
                DEBUG_NL("gpio_ingestor__message_middleware");
                DEBUG("`%s`", ingestor->name)

                // Send message by making copy of it. Use active group if address has no group in it
                emk_context_send(context, &msg);
            }
        }
    }
    return MIDDLEWARE_RESULT_HANDLED;
}