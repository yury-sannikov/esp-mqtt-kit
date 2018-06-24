#include <stddef.h>
#include <stdint.h>
#include "mqttkit/driver.h"
#include "mqttkit/ingestor.h"
#include "mqttkit/context.h"
#include "helpers/common_types.h"

DECLARE_DRIVER(gpio_ingestor)

const emk_driver_t *driver_data[] = {
    REGISTER_DRIVER(DRIVER_TYPE_INGESTOR, INGESTOR_TYPE_GPIO, gpio_ingestor),
    NULL
};


// Search for registered drivers for type and subtype. Return null if nothing has been
// found
const emk_driver_t* emk_get_driver(emk_driver_type_t type, uint16_t subtype) {
    for (const emk_driver_t **driver_it = driver_data; *driver_it; driver_it++) {
        const emk_driver_t *driver = *driver_it;
        if (driver->type == type && driver->subtype == subtype) {
            return driver;
        }
    }
    return NULL;
}

emk_driver_middleware_result_t emk_invoke_driver_middleware(const emk_config_t* config, const emk_message_t *message, emk_context_t* context) {
    for (const emk_driver_t **driver_it = driver_data; *driver_it; driver_it++) {
        const emk_driver_t *driver = *driver_it;

        bool sys_msg = EMK_IS_SYSTEM_ADDR(message->address);
        if (!sys_msg || driver->type == message->address.s.driver_type ) {
            if (driver->message_middleware) {
                emk_driver_middleware_result_t result = driver->message_middleware(config, message, context);
                if (result == MIDDLEWARE_RESULT_HANDLED) {
                    return result;
                }
            }
        }
    }
    return MIDDLEWARE_RESULT_NOT_HANDLED;
}