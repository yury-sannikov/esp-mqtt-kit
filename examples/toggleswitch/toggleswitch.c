#include <stdio.h>
#include <mqttkit/mqttkit.h>

/*
const uint8_t D0   = 16;
const uint8_t D1   = 5;
const uint8_t D2   = 4;
const uint8_t D3   = 0;
const uint8_t D4   = 2;
const uint8_t D5   = 14;
const uint8_t D6   = 12;
const uint8_t D7   = 13;
const uint8_t D8   = 15;
const uint8_t D9   = 3;
const uint8_t D10  = 1;
*/

// D1, Button ON, generate message on positive edge
 uint8_t GPIO_BUTTON_ON = 5;
// D2, Button OFF, generate message on negative edge, use the same logic address
const uint8_t GPIO_BUTTON_OFF = 4;
// D7, LED with state
const uint8_t GPIO_LED = 13;


void user_init(void) {
    const emk_ingestor_t *ingestor_data[] = {
        GPIO_INGESTOR("button ON",
            .address=EMK_COMMAND_ADDR(1),
            .config=GPIO_INGESTOR_CFG(
                .gpio = GPIO_BUTTON_ON,
                .edge = EMK_GPIO_EDGE_POS,
                .debounce = 50
            )
        ),
        GPIO_INGESTOR("button OFF",
            .address=EMK_COMMAND_ADDR(1),
            .config=GPIO_INGESTOR_CFG(
                .gpio = GPIO_BUTTON_OFF,
                .edge = EMK_GPIO_EDGE_NEG,
                .debounce = 50
            )
        ),
        NULL
    };

    const emk_actuator_t *actuator_data[] = {
        GPIO_ACTUATOR("Controlled LED",
            .address=EMK_COMMAND_ADDR(1),
            .config=GPIO_ACTUATOR_CFG(
                .gpio = GPIO_LED
            )
        ),
        NULL
    };

    const emk_group_t *groups[] = {
        MAKE_GROUP("main", 1,
            .ingestors = ingestor_data,
            .actuators = actuator_data
        ),
        NULL
    };

    emk_config_t config = {
        .groups = groups,
        .reserved_pins = 0
    };

    mqtt_kit_init(&config);
}