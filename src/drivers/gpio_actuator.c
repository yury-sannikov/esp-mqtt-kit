#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "mqttkit/driver.h"
#include "mqttkit/actuator.h"
#include "mqttkit/debug.h"
#include "mqttkit/context.h"
#include "helpers/common_types.h"

struct _actuator_data {
    // Store TickType_t at initialization to compare initial notification delay value
    TickType_t tick_at_init;
    // Transition to false when all initial notifications has been sent
    bool pending_init;
};

struct _actuator_data actuator_data;

void gpio_actuator__init_gpio(uint16_t* used_pins, const void* driver, const emk_group_t *group, const emk_config_t* config) {
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
    // Enable this pin for output, todo: config for opendrain
    gpio_enable(cfg->gpio, GPIO_OUTPUT);
    // Set GPIO initial state
    gpio_write(cfg->gpio, cfg->initial_state != 0);

    // Init actuator_data
    memset(&actuator_data, 0, sizeof(actuator_data));
    actuator_data.tick_at_init = xTaskGetTickCount();
    actuator_data.pending_init = true;
}

void gpio_actuator__gpio_iqr_block(emk_gpio_irq_block_t* irq_block, const void* driver) {
    (void)irq_block;
    (void)driver;
    ABORT("not supported");
}

void _report_actuator_status(const emk_actuator_t *actuator, const emk_group_t *group, emk_context_t* context, uint8_t value) {
    const emk_gpio_actuator_configuration* cfg = (const emk_gpio_actuator_configuration*)actuator->config;
    emk_address_t address_with_group;
    EMK_ADDRESS_MERGE_WITH_GROUP(address_with_group, *actuator->status_address, *group);
    emk_message_t msg = {
        .address = address_with_group,
        .data = (emk_data_t) {
            .type = DATA_TYPE_GPIO,
            .of.gpio.gpio_num = cfg->gpio,
            .of.gpio.gpio_val = value != 0 ? 1 : 0
        }
    };
    DEBUG_ADDR("Report actuator status to ", msg.address);
    emk_context_send(context, &msg);
}

emk_driver_middleware_result_t gpio_actuator__message_middleware(const emk_config_t* config, const emk_message_t* message, emk_context_t* context) {
    for (const emk_group_t **group_it = config->groups; *group_it; group_it++) {
        const emk_group_t *group = *group_it;
        if (group->actuators == NULL) {
            ABORT("Null actuators in group %s", group->name);
            return MIDDLEWARE_RESULT_NOT_HANDLED;
        }
        for (const emk_actuator_t **actuator_it = group->actuators; *actuator_it; actuator_it++) {
            const emk_actuator_t *actuator = *actuator_it;
            if (actuator->type != ACTUATOR_TYPE_GPIO) {
                continue;
            }

            if (!SAME_ADDRESS_DEFAULT_GROUP(message->address, *actuator->address)) {
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
            emk_context_consume(context);

            DEBUG("gpio_actuator__message_middleware");
            DEBUG("`%s` %s (%s)", actuator->name, value != 0 ? "ON" : "OFF", (value & GPIO_ACTUATOR_NOFEEDBACK) ? "NO FEEDBACK" : "w/feedback")

            gpio_write(cfg->gpio, value != 0);

            // Send actuator state update if status_address is set
            if (actuator->status_address) {
                if ((value & GPIO_ACTUATOR_NOFEEDBACK) == 0) {
                    _report_actuator_status(actuator, group, context, value);
                }
            }
        }
    }

    // Return not handled to allow side effests, such as delivering the same messate to the another logic component
    return MIDDLEWARE_RESULT_NOT_HANDLED;
}

void gpio_actuator__pool(const emk_config_t* config) {
    // Do not go over actuators if all initial messages has been fired
    if (!actuator_data.pending_init) {
        return;
    }

    bool has_pending_init = false;
    for (const emk_group_t **group_it = config->groups; *group_it; group_it++) {
        const emk_group_t *group = *group_it;
        if (group->actuators == NULL) {
            continue;
        }
        for (const emk_actuator_t **actuator_it = group->actuators; *actuator_it; actuator_it++) {
            const emk_actuator_t *actuator = *actuator_it;
            if (actuator->type != ACTUATOR_TYPE_GPIO) {
                continue;
            }
            if (actuator->status_address && actuator->initial_status_timeout != DISABLE_INITIAL_NOTIFICATION_TIMEOUT) {
                TickType_t elapsedMs = (xTaskGetTickCount() - actuator_data.tick_at_init) *  portTICK_PERIOD_MS;
                if (elapsedMs < actuator->initial_status_timeout) {
                    has_pending_init = true;
                    continue;
                }
                const emk_gpio_actuator_configuration* cfg = (const emk_gpio_actuator_configuration*)actuator->config;
                bool gpio_val = gpio_read(cfg->gpio);

                DEBUG("gpio_actuator__pool dispatch initial status");
                DEBUG("`%s` %s", actuator->name, gpio_val ? "ON" : "OFF")
                _report_actuator_status(actuator, group, NULL, (uint8_t)gpio_val);
            }
        }
    }
    // Store pending init flag, if any
    actuator_data.pending_init = has_pending_init;
}