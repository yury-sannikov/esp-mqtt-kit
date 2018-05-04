#ifndef __ESP_MQTT_KIT__TEST_ABORT_H__
#define __ESP_MQTT_KIT__TEST_ABORT_H__

extern int __test_abort_count;

void clearAbort(void);
int hasAbort(void);

#endif //__ESP_MQTT_KIT__TEST_ABORT_H__