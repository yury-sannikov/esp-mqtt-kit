#ifndef __ESP_MQTT_KIT_ADDRESS_H__
#define __ESP_MQTT_KIT_ADDRESS_H__
#include <stdint.h>

union _emk_address;
typedef union _emk_address emk_address_t;

union _emk_address {
    // User address
    struct {
        // Address. Start from 1. Zero address used by IRS and group_mask has different meaning
        uint32_t address : 15;
        // 1 - command, 0 - status
        uint32_t command_status : 1;
        // Destination group bit. Up to 16 groups.
        // Zero value means current group
        uint32_t group_mask : 16;
    } f;
    // System address
    struct {
        // Should contain zeros
        uint32_t zeroes : 16;
        // System specific type. Used by driver middleware to get their ISR messages
        uint32_t driver_type : 16;
    } s;
    uint32_t v;
};

// Create Command address with multicast bit group address
#define EMK_SYS_MIDDLEWARE_ADDR(theType) \
    (emk_address_t) { \
        .s.zeroes = 0, \
        .s.driver_type = theType \
    }


// Create Command address with default group
#define EMK_COMMAND_ADDR(theAddr) \
    (emk_address_t) { \
        .f.address = theAddr, \
        .f.command_status = 1, \
        .f.group_mask = 0 \
    }

// Create Command address with destination group
#define EMK_GROUP_COMMAND_ADDR(theAddr, grpNum) \
    (emk_address_t) { \
        .f.address = theAddr, \
        .f.command_status = 1, \
        .f.group_mask = 1 << grpNum \
    }

// Create Command address with multicast bit group address
#define EMK_MCAST_COMMAND_ADDR(theAddr, grpMask) \
    (emk_address_t) { \
        .f.address = theAddr, \
        .f.command_status = 1, \
        .f.group_mask = grpMask & 0xFFFF \
    }

// Create Status address with default group
#define EMK_STATUS_ADDR(theAddr) \
    (emk_address_t) { \
        .f.address = theAddr, \
        .f.command_status = 0, \
        .f.group_mask = 0 \
    }

// Create Status address with default group
#define EMK_GROUP_STATUS_ADDR(theAddr, grpNum) \
    (emk_address_t) { \
        .f.address = theAddr, \
        .f.command_status = 0, \
        .f.group_mask = 1 < grpNum \
    }

// Create Command address with multicast bit group address
#define EMK_MCAST_STATUS_ADDR(theAddr, grpMask) \
    (emk_address_t) { \
        .f.address = theAddr, \
        .f.command_status = 0, \
        .f.group_mask = grpMask & 0xFFFF \
    }


#endif //__ESP_MQTT_KIT_ADDRESS_H__