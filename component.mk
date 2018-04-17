INC_DIRS += $(esp_mqtt_kit_ROOT)/include

esp_mqtt_kit_INC_DIR = $(esp_mqtt_kit_ROOT)/include $(esp_mqtt_kit_ROOT)/src
esp_mqtt_kit_SRC_DIR = $(esp_mqtt_kit_ROOT)/src

$(eval $(call component_compile_rules,esp_mqtt_kit))

ifeq ($(ESP_MQTT_KIT_DEBUG),1)
esp_mqtt_kit_CFLAGS += -DESP_MQTT_KIT_DEBUG
endif
