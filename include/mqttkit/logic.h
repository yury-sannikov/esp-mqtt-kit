#ifndef __ESP_MQTT_KIT_LOGIC_H__
#define __ESP_MQTT_KIT_LOGIC_H__

#include "mqttkit/context.h"

struct _emk_logic;
struct _emk_logic_slot;

typedef struct _emk_logic emk_logic_t;
typedef struct _emk_logic_slot emk_logic_slot_t;

// Slot number when mqttkit initialize logic block
#define SLOT_ID_INIT (uint32_t)-1

#define LOGIC_INIT_FAILURE_RESULT NULL
#define LOGIC_INIT_SUCCESS_RESULT (void*)1


struct _emk_logic {
    // Logic block name
    const char* name;
    // Block slots definition
    const emk_logic_slot_t** slots;
    // logic block handler
    //   slot_id - address slot identifier which has meaning only within particular logic block implementation
    //   config - logic configuration
    //   handler_context - data, previously returned from the logic_handler function. Might be used to hold internal state for the particular logic instance
    //   payload - message data
    //   call_context - execution context
    void* (*logic_handler)(uint32_t slot_id, const void* config, void* handler_context, const emk_data_t* payload, emk_context_t* call_context);
    // Logic block configuration
    const void* config;
};

struct _emk_logic_slot {
    // Slot id
    uint32_t slot_id;
    // Logic slot name
    const char* name;
    // Slot address
    const emk_address_t* address;
};


#define LOGIC_BLOCK(aName, aSlots, aHandler) \
    &(emk_logic_t) { \
        .name=aName, \
        .slots=aSlots, \
        .logic_handler=aHandler \
    }


#define LOGIC_SLOT(aName, anAddress, ...) \
    &(emk_logic_slot_t) { \
        .name=aName, \
        .address=anAddress, \
        ##__VA_ARGS__ \
    }

#endif