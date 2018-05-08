#ifndef __ESP_MQTT_KIT_DRIVER_H__
#define __ESP_MQTT_KIT_DRIVER_H__
#include <stdint.h>
#include "ingestor.h"
#include "configurator.h"

struct _emk_driver;
typedef struct _emk_driver emk_driver_t;

typedef enum {
    DRIVER_TYPE_INGESTOR = 1
} emk_driver_type_t;

struct _emk_driver {
    // Driver type
    emk_driver_type_t type;

    // Driver subtype
    uint16_t subtype;

    // Check GPIO pin usage. Might update used_pins if pin can not be reused
    void (*check_gpio)(uint16_t* /*used_pins*/, const emk_ingestor_t* /*ingestor*/);

    // Driver might either update IRQ block or check that particular GPIO has been used by 
    // gpio ingestor
    void (*gpio_iqr_block)(emk_gpio_irq_block_t* gpio_irq_block, const emk_ingestor_t* ingestor);
    
};

#define DECLARE_DRIVER(prefix) \
    void prefix##__check_gpio(uint16_t* used_pins, const emk_ingestor_t* ingestor); \
    void prefix##__gpio_iqr_block(emk_gpio_irq_block_t* gpio_irq_block, const emk_ingestor_t* ingestor);


#define REGISTER_DRIVER(theType, theSubtype, prefix) \
    &(emk_driver_t) { \
        .type = theType, \
        .subtype = theSubtype, \
        .check_gpio = prefix##__check_gpio, \
        .gpio_iqr_block = prefix##__gpio_iqr_block \
    }


// Get one of the registered drivers
const emk_driver_t* emk_get_driver(emk_driver_type_t type, uint16_t subtype);



#endif // __ESP_MQTT_KIT_DRIVER_H__