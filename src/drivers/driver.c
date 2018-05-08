#include <stddef.h>
#include <stdint.h>
#include "mqttkit/driver.h"
#include "mqttkit/ingestor.h"

DECLARE_DRIVER(ingestor_gpio)

const emk_driver_t *driver_data[] = {
    REGISTER_DRIVER(DRIVER_TYPE_INGESTOR, INGESTOR_TYPE_GPIO, ingestor_gpio),
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

