#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "mqttkit/configurator.h"
#include "mqttkit/debug.h"
#include "mqttkit/driver.h"
#include "mqttkit/message.h"
#include "helpers/common_types.h"

uint32_t IRAM __message_send_overflow_counter = 0;


void emk_context_init(emk_context_t* context, const emk_address_t* address) {
    context->source_address = *address;
    context->message_consumed = false;
}

void emk_context_send(emk_context_t* context, const emk_message_t* message) {
    context->message_consumed = true;
    if (errQUEUE_FULL == xQueueSendFromISR(__gpio_irq_block.queue, message, NULL)) {
        ++__message_send_overflow_counter;
    }
}

void emk_context_cleanup(emk_context_t* context) {
    if (!context->message_consumed) {
        if (EMK_IS_SYSTEM_ADDR(context->source_address)) {
            printf("  System message was not consumed. Drvier: %X\n", context->source_address.s.driver_type);
        } else {
            printf("  Message was not consumed. [A:%X C:%X G:%x]\n",
                context->source_address.f.address,
                context->source_address.f.command_status,
                context->source_address.f.group_mask
            );
        }
    }
}
