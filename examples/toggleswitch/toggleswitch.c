#include <stdio.h>
#include <esp8266.h>
#include <FreeRTOS.h>
#include "esp/uart.h"
#include <mqttkit/mqttkit.h>

void ingestor_init(void) {
    printf("Ingestor init called");
}

void user_init(void) {
    uart_set_baud(0, 115200);

    mqtt_kit_init(NULL);
}