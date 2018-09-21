#ifndef __ESP_MQTT_KIT_CONTEXT_H__
#define __ESP_MQTT_KIT_CONTEXT_H__
#include "helpers/common_types.h"
#include "typedefs.h"

struct _emk_context;

struct _emk_context {
    bool message_consumed;
    emk_address_t source_address;
};

// Initialize emk_context_t
void emk_context_init(emk_context_t* context, const emk_address_t* address);
// Send message within specified context
void emk_context_send(emk_context_t* context, const emk_message_t* message);
// Send message within specified context to the current group
void emk_context_send_with_group(emk_context_t* context, const emk_group_t* group, emk_message_t* message);
// Check context and give warnings if any
void emk_context_cleanup(emk_context_t* context);
// Mark context as consumed. may used in driver middleware
void emk_context_consume(emk_context_t* context);


#endif //__ESP_MQTT_KIT_CONTEXT_H__