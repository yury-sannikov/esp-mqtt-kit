#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "logic/toggleswitch.h"
#include "helpers/common_types.h"
#include "mqttkit/logic.h"
#include "mqttkit/debug.h"

void report_state(emk_context_t* call_context, const emk_group_t* group, const emk_address_t *address, uint8_t state) {
    emk_message_t msg = {
        .address = *address,
        .data = (emk_data_t) {
            .type = DATA_TYPE_B8,
            .of.b8 = state
        }
    };
    emk_context_send_with_group(call_context, group, &msg);
}

void* toggle_switch_logic_handler(const emk_group_t* group, uint32_t slot_id, const void* slot_config, void* handler_context, const emk_data_t* payload, emk_context_t* call_context) {
    (void)payload;

    if (slot_id == SLOT_ID_INIT) {
        // Create switch state
        int* context = malloc(sizeof(int));
        *context = TOGGLESWITCH_OFF;
        return context;
    }

    const emk_address_t *report_address = (const emk_address_t *)slot_config;

    switch (slot_id)
    {
        case TOGGLESWITCH_TURNON:
            DEBUG("toggle_switch_logic_handler: TOGGLESWITCH_TURNON, report_state >> TOGGLESWITCH_ON");
            report_state(call_context, group, report_address, TOGGLESWITCH_ON);
            break;
        case TOGGLESWITCH_TURNOFF:
            DEBUG("toggle_switch_logic_handler: TOGGLESWITCH_TURNOFF, report_state >> TOGGLESWITCH_OFF");
            report_state(call_context, group, report_address, TOGGLESWITCH_OFF);
            break;
        case TOGGLESWITCH_TOGGLE: {
            uint8_t state;
            state = (*(int*)handler_context & 0xFF) ? 0 : 1;
            DEBUG("toggle_switch_logic_handler: TOGGLESWITCH_TOGGLE, report_state >> %d", state);
            report_state(call_context, group, report_address, state);
            break;
        }
        case TOGGLESWITCH_SET:
            if (payload->type != DATA_TYPE_B8) {
                ABORT("TOGGLESWITCH_SET expect DATA_TYPE_B8");
            }
            DEBUG("toggle_switch_logic_handler: TOGGLESWITCH_SET, report_state >> %d", payload->of.b8);
            report_state(call_context, group, report_address, payload->of.b8);
            break;
        case TOGGLESWITCH_FEEDBACK: {
            int new_state = 0;
            switch(payload->type) {
                case DATA_TYPE_GPIO: new_state = payload->of.gpio.gpio_val; break;
                case DATA_TYPE_B8: new_state = payload->of.b8; break;
                default: ABORT("TOGGLESWITCH_FEEDBACK has unknow data type");
            }
            *((int*)handler_context) = new_state;
            DEBUG("toggle_switch_logic_handler: TOGGLESWITCH_FEEDBACK, set_state << %d", new_state);
            break;
        }
        default:
            break;
    }

    return handler_context;
}
