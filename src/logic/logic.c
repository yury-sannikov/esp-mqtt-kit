#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mqttkit/configurator.h"

#include "mqttkit/debug.h"
#include "mqttkit/logic.h"

void** _logic_context;

void _init_logic_blocks(const emk_config_t* cfg) {
    uint32_t total_groups = 0;
    for (const emk_group_t **group_it = cfg->groups; *group_it; ++total_groups, ++group_it);

    _logic_context = malloc(total_groups * sizeof(void*));
    memset(_logic_context, 0, total_groups * sizeof(void*));

    void** current_logic_context = _logic_context;
    for (const emk_group_t **group_it = cfg->groups; *group_it; ++group_it, ++current_logic_context) {
        uint32_t total_logic_blocks_in_group = 0;
        const emk_group_t *group = *group_it;

        if (!group->logic) {
            continue;
        }
        for (const emk_logic_t **logic_it = group->logic; *logic_it; ++logic_it, ++total_logic_blocks_in_group);

        *current_logic_context = malloc(total_logic_blocks_in_group * sizeof(void*));
        memset(*current_logic_context, 0, total_logic_blocks_in_group * sizeof(void*));

        void** current_context = *current_logic_context;
        for (const emk_logic_t **logic_it = group->logic; *logic_it; ++logic_it, ++current_context) {
            const emk_logic_t *logic = *logic_it;
            void* context = logic->logic_handler(SLOT_ID_INIT, logic->config, NULL, NULL, NULL);
            if (!context) {
                ABORT("Failed to initialize logic block `%s` in group `%s`", logic->name, group->name);
                return;
            }
            *current_context = context;
        }
    }
}

// void emk_process_logic(const emk_config_t* config, const emk_message_t *message, emk_context_t* context) {
//     void** current_logic_context = _logic_context;
//     for (const emk_group_t **group_it = cfg->groups; *group_it; ++group_it, ++current_logic_context) {
//         const emk_group_t *group = *group_it;
//         if (!group->logic) {
//             continue;
//         }

//         void** current_context = *current_logic_context;
//         for (const emk_logic_t **logic_it = group->logic; *logic_it; ++logic_it, ++current_context) {
//             const emk_logic_t *logic = *logic_it;

//             void* context = logic->logic_handler(SLOT_ID_INIT, logic->config, *current_context, &message->data, context);

//             if (context) {
//                 *current_context = context;
//             }
//         }
//     }

// }
