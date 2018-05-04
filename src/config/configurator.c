#include <stdio.h>
#include <stdlib.h>
#include "mqttkit/mqttkit.h"
#include "mqttkit/debug.h"

emk_config_t* emk_create_configuration(void) {
    emk_config_t* config = malloc(sizeof(emk_config_t));
    return config;
}

void emk_initialize(emk_config_t* cfg) {
    if (!cfg) {
        ABORT("emk_add_gpio_ingestors has null emk_config_t supplied");
        return;
    }
    // uint32_t gpioMask = 0;
    // for (emk_ingestor_t **ingestor_it = ingestors; *ingestor_it; ingestor_it++) {
    //     emk_ingestor_t *ingestor = *ingestor_it;
    //     printf("add %s\n", ingestor->name);
    //     // uint32_t bitNumber = 1 << ingestor->gpio;
    //     // if (gpioMask & bitNumber) {
    //     //     ABORT("emk_add_gpio_ingestors has reused GPIO %d for %s", ingestor->gpio, ingestor->name);
    //     // }
    // }

}
