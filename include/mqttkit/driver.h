#ifndef __ESP_MQTT_KIT_DRIVER_H__
#define __ESP_MQTT_KIT_DRIVER_H__
#include <stdint.h>
#include "ingestor.h"
#include "configurator.h"


#define IRQ_LAST_TRIGGERED_DEFAULT_TIME 0xFFFFFFFF

struct _emk_driver;
typedef struct _emk_driver emk_driver_t;

typedef enum {
    DRIVER_TYPE_INGESTOR = 1
} emk_driver_type_t;

typedef enum {
    MIDDLEWARE_RESULT_NOT_HANDLED = 0,
    MIDDLEWARE_RESULT_HANDLED = 1
} emk_driver_middleware_result_t;

struct _emk_driver {
    // Driver type
    emk_driver_type_t type;

    // Driver subtype
    uint16_t subtype;

    // Check GPIO pin usage. Might update used_pins if pin can not be reused
    void (*check_gpio)(uint16_t* /*used_pins*/, const emk_ingestor_t* /*ingestor*/, const emk_group_t* /*group*/, const emk_config_t* /* config */);

    // Driver might either update IRQ block or check that particular GPIO has been used by
    // gpio ingestor
    void (*gpio_iqr_block)(emk_gpio_irq_block_t* gpio_irq_block, const emk_ingestor_t* ingestor);

    // Each logic message goes through drivers middleware. Middleware can handle it
    // and ignore or dispatch another message
    emk_driver_middleware_result_t (*message_middleware)(const emk_config_t* config, void *message);

};

#define DECLARE_DRIVER(prefix) \
    void prefix##__check_gpio(uint16_t* used_pins, const emk_ingestor_t* ingestor, const emk_group_t *group, const emk_config_t* config); \
    void prefix##__gpio_iqr_block(emk_gpio_irq_block_t* gpio_irq_block, const emk_ingestor_t* ingestor); \
    emk_driver_middleware_result_t prefix##__message_middleware(const emk_config_t* config, void *message);


#define REGISTER_DRIVER(theType, theSubtype, prefix) \
    &(emk_driver_t) { \
        .type = theType, \
        .subtype = theSubtype, \
        .check_gpio = prefix##__check_gpio, \
        .gpio_iqr_block = prefix##__gpio_iqr_block, \
        .message_middleware = prefix##__message_middleware \
    }


// Get one of the registered drivers
const emk_driver_t* emk_get_driver(emk_driver_type_t type, uint16_t subtype);

// Declare emk_gpio_irq_block_t global variable to be accessible from ISR
extern IRAM_DATA emk_gpio_irq_block_t  __gpio_irq_block;

// Driver isr for gpio ingestor
void IRAM gpio_ingestor_interrupt_handler(uint8_t gpio_num);


#endif // __ESP_MQTT_KIT_DRIVER_H__