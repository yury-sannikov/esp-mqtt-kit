#ifndef __ESP_MQTT_KIT_INGESTOR_H__
#define __ESP_MQTT_KIT_INGESTOR_H__

#include <stdint.h>
#include "address.h"

struct _emk_ingestor;
struct _gpio_ingestor_configuration;
typedef struct _emk_ingestor emk_ingestor_t;
typedef struct _gpio_ingestor_configuration emk_gpio_ingestor_configuration;


// Contain a GPIO pin interrupt edge
typedef enum {
    EMK_GPIO_EDGE_POS = 1,
    EMK_GPIO_EDGE_NEG = 2,
    EMK_GPIO_EDGE_BOTH = 4
} emk_edge_t;

typedef enum _emk_ingestor_type {
    INGESTOR_TYPE_GPIO = 1,
    iNGESTOR_TYPE_DS18B20 = 2
} emk_ingestor_type_t;

struct _gpio_ingestor_configuration {
    // GPIO port number
    uint8_t gpio;
    // GPIO interrupt trigger
    emk_edge_t edge;
    // software debounce, msec
    uint32_t debounce;    
};

#define GPIO_INGESTOR_CFG(...) \
    &(emk_gpio_ingestor_configuration) { \
        .debounce=50, \
        ##__VA_ARGS__ \
    }

struct _emk_ingestor {
    // Ingestor name, debugging only
    const char* name;
    // Ingestor type
    emk_ingestor_type_t type;
    // Address which will be used for status update
    emk_address_t address;
    // Ingestor specific configuration
    void* config;
};

#define GPIO_INGESTOR(aName, ...) \
    &(emk_ingestor_t) { \
        .name=aName, \
        .type=INGESTOR_TYPE_GPIO, \
        ##__VA_ARGS__ \
    }

#endif // __ESP_MQTT_KIT_INGESTOR_H__
