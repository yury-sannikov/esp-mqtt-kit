#include <stdio.h>
#include <mqttkit/mqttkit.h>

#define D0  16
#define D1  5
#define D2  4
#define D3  0
#define D4  2
#define D5  14
#define D6  12
#define D7  13
#define D8  15
#define D9  3
#define D10 1

// D1, Button ON, generate message on positive edge
#define GPIO_BUTTON_ON D1
// D2, Button OFF, generate message on negative edge, use the same logic address
#define GPIO_BUTTON_OFF D2
// D7, LED with state
#define GPIO_LED D7

// Ingestor configurations data.
// Please do not define it inside user_init, i.e. on a stack
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

// Actuator configurations data.
const emk_actuator_t *actuator_data[] = {
    GPIO_ACTUATOR("Controlled LED",
        .address=EMK_COMMAND_ADDR(1),
        .config=GPIO_ACTUATOR_CFG(
            .gpio = GPIO_LED
        )
    ),
    NULL
};

// Groups data with ingestors and actuators data
const emk_group_t *groups[] = {
    MAKE_GROUP("main", 0,
        .ingestors = ingestor_data,
        .actuators = actuator_data
    ),
    NULL
};

// Configuration object
const emk_config_t config = {
    .groups = groups,
    .reserved_pins = 0
};

void user_init(void) {
    mqtt_kit_init(&config);
}