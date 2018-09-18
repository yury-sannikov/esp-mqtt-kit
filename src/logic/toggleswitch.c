#include <stdbool.h>
#include <stddef.h>
// #include <stdio.h>
#include "logic/toggleswitch.h"
// #include "mqttkit/configurator.h"
// #include "mqttkit/debug.h"
// #include "mqttkit/driver.h"
// #include "mqttkit/message.h"
#include "helpers/common_types.h"


void* toggle_switch_logic_handler(uint32_t slot_id, const void* slot_config, void* handler_context, const emk_data_t* payload, emk_context_t* call_context) {
    (void)slot_config;
    (void)payload;
    (void)call_context;

    if (slot_id == SLOT_ID_INIT) {
        return LOGIC_INIT_SUCCESS_RESULT;
    }

    return handler_context;
}
