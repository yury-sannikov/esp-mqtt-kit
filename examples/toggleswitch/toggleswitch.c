#include <stdio.h>
#include <esp8266.h>
#include <FreeRTOS.h>
#include "esp/uart.h"
#include <mqttkit/mqttkit.h>

// #include "espressif/esp_common.h"
#include "task.h"
#include "queue.h"

//TODO: https://github.com/drasko/esp-open-rtos-examples/blob/master/mainflux_button/mainflux_button.c

void ingestor_init(void) {
    printf("Ingestor init called");
}

const int gpioinp = 5; // D1
const gpio_inttype_t int_type = GPIO_INTTYPE_EDGE_POS;
void gpio_intr_handler(uint8_t gpio_num);

void buttonIntTask(void *pvParameters)
{
    printf("Waiting for button press interrupt on gpio %d...\r\n", gpioinp);
    QueueHandle_t *tsqueue = (QueueHandle_t *)pvParameters;
    gpio_set_interrupt(gpioinp, int_type, gpio_intr_handler);

    uint32_t last = 0;
    while(1) {
        uint32_t button_ts;
        xQueueReceive(*tsqueue, &button_ts, portMAX_DELAY);
        button_ts *= portTICK_PERIOD_MS;
        printf("INTR at %dms\r\n", button_ts);
        if(last < button_ts-200) {
            printf("Button interrupt fired at %dms\r\n", button_ts);
            last = button_ts;
        }
    }
}

static QueueHandle_t tsqueue;

void gpio_intr_handler(uint8_t gpio_num)
{
    uint32_t now = xTaskGetTickCountFromISR();
    xQueueSendToBackFromISR(tsqueue, &now, NULL);
}


void user_init(void) {
    uart_set_baud(0, 115200);

    mqtt_kit_init(NULL);

    gpio_enable(gpioinp, GPIO_INPUT);
    tsqueue = xQueueCreate(20, sizeof(uint32_t));
    xTaskCreate(buttonIntTask, "buttonIntTask", 256, &tsqueue, 2, NULL);

}