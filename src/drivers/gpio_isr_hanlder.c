#include "helpers/common_types.h"
#include "mqttkit/driver.h"

void IRAM gpio_ingestor_interrupt_handler(uint8_t gpio_num) {
    // check that triggered gpio interrupt was active
    if ((__gpio_irq_block.active_pins & BIT(gpio_num)) == 0) {
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
        TickType_t diff = current_time - last_irq_time;
        // Check clock cycled and update diff
        if (last_irq_time > current_time) {
            TickType_t max_but_one = ~((TickType_t) 0) - 1;
            diff = last_irq_time - max_but_one + current_time;
        }
        // Check if we have to debounce this occurence
        if (diff < __gpio_irq_block.debouce_values[gpio_num]) {
            SET_GPIO_ISR_STATUS(-3);
            return;
        }

    }
    __gpio_irq_block.last_irq[gpio_num] = current_time;



    SET_GPIO_ISR_STATUS(0);
}
