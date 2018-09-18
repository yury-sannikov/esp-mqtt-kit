/* The classic "blink" example
 *
 * This sample code is in the public domain.
 */
#include <stdlib.h>
#include <FreeRTOS.h>

#include "espressif/esp_common.h"
#include "task.h"
#include "queue.h"
#include "esp8266.h"
#include "esp/uart.h"

#include "mqttkit/mqttkit.h"
#include "mqttkit/driver.h"
#include "mqttkit/debug.h"


emk_task_parameter_block_t receiver_task_parameter_block;

typedef void (*receiver_task_t)(void *pvParameters);

void mqtt_kit_init(const emk_config_t *config)
{
    uart_set_baud(0, 115200);

    DEBUG("calling emk_initialize");
    emk_initialize(config);

    receiver_task_parameter_block.config = config;
    receiver_task_parameter_block.irq_block = &__gpio_irq_block;

    DEBUG("calling xTaskCreate");
    BaseType_t xReturned = xTaskCreate((receiver_task_t)receiver_task, "receiver_task", 768, &receiver_task_parameter_block, 2, NULL);
    if( xReturned != pdPASS )
    {
        ABORT("unable to start receiver_task");
    }
}
