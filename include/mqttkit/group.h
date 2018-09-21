#ifndef __ESP_MQTT_KIT_GROUP_H__
#define __ESP_MQTT_KIT_GROUP_H__
#include <stdint.h>
#include "typedefs.h"

struct  _emk_group {
    const char* name;
    // Group address, 1 to 15.
    uint8_t     group_address;
    // Ingestors array
    const emk_ingestor_t **ingestors;
    // Actuators array
    const emk_actuator_t **actuators;
    // Logic array
    const emk_logic_t **logic;
};

#define MAKE_GROUP(theName, addr, ...) \
    &(emk_group_t) { \
        .name=theName, \
        .group_address=(addr) & 0xF, \
        ##__VA_ARGS__ \
    }


#endif //__ESP_MQTT_KIT_GROUP_H__
