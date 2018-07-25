#include <stdbool.h>
#include <stddef.h>
#include "helpers/common_types.h"
#include "mqttkit/driver.h"
#include "mqttkit/message.h"

uint32_t __gpio_isr_queue_overflow_counter = 0;

void IRAM _send_status(uint8_t gpio_num, bool value);
void IRAM gpio_ingestor_interrupt_handler(uint8_t gpio_num) {
    // check that triggered gpio interrupt was active
    uint16_t gpio_mask = BIT(gpio_num);
    if ((__gpio_irq_block.active_pins & gpio_mask) == 0) {
        SET_GPIO_ISR_STATUS(-1);
        return;
    }
    if (gpio_num & 0xF0) {
        SET_GPIO_ISR_STATUS(-2);
        return;
    }
    TickType_t last_irq_time =  __gpio_irq_block.last_irq[gpio_num];
    TickType_t current_time = xTaskGetTickCountFromISR();
    if (IRQ_LAST_TRIGGERED_DEFAULT_TIME != last_irq_time) {
        // Unsigned TickType_t will handle overlaps pretty well
        TickType_t diff = current_time - last_irq_time;
        // Check if we have to debounce this occurence
        if (diff <= __gpio_irq_block.debouce_values[gpio_num]) {
            SET_GPIO_ISR_STATUS(-4);
            return;
        }

    }
    if (IRQ_LAST_TRIGGERED_DEFAULT_TIME == current_time) {
        current_time = IRQ_LAST_TRIGGERED_DEFAULT_TIME + 1;
    }
    __gpio_irq_block.last_irq[gpio_num] = current_time;

    bool value = gpio_read(gpio_num);
    if (value && (__gpio_irq_block.pos_edge & gpio_mask)) {
        _send_status(gpio_num, value);
        SET_GPIO_ISR_STATUS(1);
    }

    if (!value && (__gpio_irq_block.neg_edge & gpio_mask)) {
        _send_status(gpio_num, value);
        SET_GPIO_ISR_STATUS(2);
    }
}

void IRAM _send_status(uint8_t gpio_num, bool value) {
    emk_message_t msg = {
        .address = EMK_SYS_MIDDLEWARE_ADDR(DRIVER_TYPE_INGESTOR),
        .data = (emk_data_t) {
            .type = DATA_TYPE_GPIO,
            .of.gpio = (emk_gpio_data_t) {
                .gpio_num = gpio_num,
                .gpio_val = value
            }
        }
    };
    if (errQUEUE_FULL == xQueueSendFromISR(__gpio_irq_block.queue, &msg, NULL)) {
        ++__gpio_isr_queue_overflow_counter;
    }
}