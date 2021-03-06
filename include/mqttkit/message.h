#ifndef __ESP_MQTT_KIT_MESSAGE_H__
#define __ESP_MQTT_KIT_MESSAGE_H__
#include <stdint.h>
#include "mqttkit/address.h"
#include "mqttkit/data_types.h"
#include "mqttkit/typedefs.h"

struct _emk_message;

struct _emk_message {
    emk_address_t address;
    emk_data_t data;
};

#define TEST_ASSERT_EQUAL_MSG(theExpected, theActual) \
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&((theExpected).address), &((theActual).address), sizeof(emk_address_t), "(address mismatch)"); \
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&((theExpected).data), &((theActual).data), sizeof(emk_data_t), "(data mismatch)");

#endif //__ESP_MQTT_KIT_MESSAGE_H__