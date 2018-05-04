#ifndef __ESP_MQTT_KIT_CFG_H__
#define __ESP_MQTT_KIT_CFG_H__

#include <stdint.h>
#include "address.h"
#include "group.h"
#include "ingestor.h"

typedef struct _emk_config emk_config_t;

struct _emk_config {
    emk_group_t **groups;
};

void emk_initialize(emk_config_t* cfg);


#endif // __ESP_MQTT_KIT_CFG_H__
