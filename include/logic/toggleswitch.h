#ifndef __ESP_MQTT_KIT_LOGIC_TOGGLESWITCH_H__
#define __ESP_MQTT_KIT_LOGIC_TOGGLESWITCH_H__
#include <stdint.h>
#include "mqttkit/data_types.h"
#include "mqttkit/context.h"
#include "mqttkit/logic.h"

void* toggle_switch_logic_handler(uint32_t slot_id, const void* slot_config, void* handler_context, const emk_data_t* payload, emk_context_t* call_context);

typedef enum {
    // Message to this slot will set TS to the ON state
    TOGGLESWITCH_TURNON,
    // Message to this slot will set TS to the OFF state
    TOGGLESWITCH_TURNOFF,
    // Message to this slot will toggle TS state
    TOGGLESWITCH_TOGGLE,
    // Message to this slot will set TS to the state from message data
    TOGGLESWITCH_SET,
    // This slot is used to set internal state of the TS w/o dispatching any messages
    // Should be used along with gpio actuator feedback address / initial actuator state message
    // Toggleswitch will never update it's internal state for other slots call but this one.
    // Feedback status is used as a reference for TOGGLESWITCH_TOGGLE slot
    TOGGLESWITCH_FEEDBACK
} toggleswitch_slot_t;

#define TOGGLESWITCH_BLOCK(aName, aDispatchAddress, aSlots) \
    &(emk_logic_t) { \
        .name=aName, \
        .slots=aSlots, \
        .logic_handler=toggle_switch_logic_handler, \
        .config=aDispatchAddress \
    }


#define TOGGLESWITCH_SLOT_TURNON(aName, anAddress) \
    &(emk_logic_slot_t) { \
        .slot_id=TOGGLESWITCH_TURNON,\
        .name=aName, \
        .address=anAddress \
    }

#define TOGGLESWITCH_SLOT_TURNOFF(aName, anAddress) \
    &(emk_logic_slot_t) { \
        .slot_id=TOGGLESWITCH_TURNOFF,\
        .name=aName, \
        .address=anAddress \
    }

#define TOGGLESWITCH_SLOT_TOGGLE(aName, anAddress) \
    &(emk_logic_slot_t) { \
        .slot_id=TOGGLESWITCH_TOGGLE,\
        .name=aName, \
        .address=anAddress \
    }

#define TOGGLESWITCH_SLOT_SET(aName, anAddress) \
    &(emk_logic_slot_t) { \
        .slot_id=TOGGLESWITCH_SET,\
        .name=aName, \
        .address=anAddress \
    }

#define TOGGLESWITCH_SLOT_FEEDBACK(aName, anAddress) \
    &(emk_logic_slot_t) { \
        .slot_id=TOGGLESWITCH_FEEDBACK,\
        .name=aName, \
        .address=anAddress \
    }

#endif // __ESP_MQTT_KIT_LOGIC_TOGGLESWITCH_H__