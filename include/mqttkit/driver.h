#ifndef __ESP_MQTT_KIT_DRIVER_H__
#define __ESP_MQTT_KIT_DRIVER_H__
#include <stdint.h>
#include "ingestor.h"
#include "configurator.h"
#include "context.h"
#include "message.h"
#include "typedefs.h"

#define IRQ_LAST_TRIGGERED_DEFAULT_TIME 0xDEADBEEF

struct _emk_driver;

typedef enum {
    DRIVER_TYPE_INGESTOR = 1,
    DRIVER_TYPE_ACTUATOR = 2
} emk_driver_type_t;

#define DEBUG_DRIVER_TYPE(theType) \
    switch((theType)) {\
        case DRIVER_TYPE_INGESTOR: printf("DRIVER_TYPE_INGESTOR"); break;\
        case DRIVER_TYPE_ACTUATOR: printf("DRIVER_TYPE_ACTUATOR"); break;\
        default: printf("Unknown DRIVER TYPE of %d", (theType)); break;\
    }

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
    void (*init_gpio)(uint16_t* /*used_pins*/, const void* /*driver*/, const emk_group_t* /*group*/, const emk_config_t* /* config */);

    // Driver might either update IRQ block or check that particular GPIO has been used by
    // gpio ingestor
    void (*gpio_iqr_block)(emk_gpio_irq_block_t* gpio_irq_block, const void* driver);

    // Each logic message goes through drivers middleware. Middleware can handle it
    // and ignore or dispatch another message
    emk_driver_middleware_result_t (*message_middleware)(const emk_config_t* config, const emk_message_t* message, emk_context_t* context);

    // This function will be called each LOGIC_TIMER_RESOLUTION_MS or after message has been received
    // Used to handle non-critical timeouts in driver logic such as initial state notifications
    void (*pool)(const emk_config_t* config);

};

#define DECLARE_DRIVER(prefix) \
    void prefix##__init_gpio(uint16_t* used_pins, const void* driver, const emk_group_t *group, const emk_config_t* config); \
    void prefix##__gpio_iqr_block(emk_gpio_irq_block_t* gpio_irq_block, const void* driver); \
    emk_driver_middleware_result_t prefix##__message_middleware(const emk_config_t* config, const emk_message_t* message, emk_context_t* context); \
    void prefix##__pool(const emk_config_t* config);


#define REGISTER_DRIVER(theType, theSubtype, prefix) \
    &(emk_driver_t) { \
        .type = theType, \
        .subtype = theSubtype, \
        .init_gpio = prefix##__init_gpio, \
        .gpio_iqr_block = prefix##__gpio_iqr_block, \
        .message_middleware = prefix##__message_middleware, \
        .pool = prefix##__pool \
    }


// Get one of the registered drivers
const emk_driver_t* emk_get_driver(emk_driver_type_t type, uint16_t subtype);

// Declare emk_gpio_irq_block_t global variable to be accessible from ISR
extern emk_gpio_irq_block_t  __gpio_irq_block;

// Driver isr for gpio ingestor
void IRAM gpio_ingestor_interrupt_handler(uint8_t gpio_num);

// Call driver's middleware. Driver may consume message, optionally send another one through context
// If message consumed, MIDDLEWARE_RESULT_HANDLED should be returned.
// New message goes back into queue so other middleware have a chance to process that new message
emk_driver_middleware_result_t emk_invoke_driver_middleware(const emk_config_t* config, const emk_message_t *message, emk_context_t* context);

// Call drivers __pool method
void emk_pool_drivers(const emk_config_t* config);

#endif // __ESP_MQTT_KIT_DRIVER_H__