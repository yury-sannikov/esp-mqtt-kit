#include <stdint.h>
#include <stdbool.h>
#include "mqttkit/message.h"
// This file include RTOS specific data types
// Unit tests has their own verison defining used types so tests has no dependency with RTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Define as empty as it's using only for tests
#define SET_GPIO_ISR_STATUS(status)
