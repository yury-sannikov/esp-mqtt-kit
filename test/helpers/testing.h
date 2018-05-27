#ifndef __ESP_MQTT_KIT__TESTING_H__
#define __ESP_MQTT_KIT__TESTING_H__

extern int __test_abort_count;

// ABORT stuff
void clearAbort(void);
int hasAbort(void);
int getAbortsCount(void);

// RTOS gpio_set_interrupt test
void test_clear_gpio_set_interrupt(void);
void assert_gpio_set_interrupt(int gpio_mask, int call_count);


#endif //__ESP_MQTT_KIT__TESTING_H__