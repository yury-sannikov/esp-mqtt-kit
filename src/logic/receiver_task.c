#include "mqttkit/configurator.h"
#include "mqttkit/debug.h"
#include "mqttkit/driver.h"
#include "mqttkit/message.h"
#include "helpers/common_types.h"

int receiver_task( void *pvParameters )
{
    DEBUG("receiver_task started");
    const emk_task_parameter_block_t* pb = (const emk_task_parameter_block_t*)pvParameters;
    if (!pb) {
        ABORT("receiver_task has null parameter block received");
        return -1;
    }
    if (!pb->irq_block->queue) {
        ABORT("receiver_task has no queue set");
        return -2;
    }
    DEBUG("receiver_task check pass");
    printf("__gpio_irq_block.queue = %X", __gpio_irq_block.queue);
    emk_message_t msg;
    for( ;; ) {
        if (xQueueReceive(__gpio_irq_block.queue, &msg, LOGIC_TIMER_RESOLUTION_MS / portTICK_PERIOD_MS)) {
            emk_context_t context;
            DEBUG("");
            DEBUG_NL("receiver_task ");
            DEBUG_ADDR("", msg.address);
            DEBUG_DATA("", msg.data);

            emk_context_init(&context, &msg.address);

            if (MIDDLEWARE_RESULT_HANDLED != emk_invoke_driver_middleware(pb->config, &msg, &context)) {
                // invoke logic
            }
            // Check & cleanup context
            emk_context_cleanup(&context);
        }

        // HERE: Process timers

#ifdef SYSTEM_UNDER_TEST
        break;
#endif
    }
    return 0;
}