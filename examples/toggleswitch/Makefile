PROGRAM = toggleswitch
SDK_PATH = $(abspath ../../components/rtos)
EXTRA_COMPONENTS = \
	extras/paho_mqtt_c \
	extras/mbedtls \
	$(abspath ../../)

FLASH_SIZE ?= 32

EXTRA_CFLAGS += -I..

include $(SDK_PATH)/common.mk

monitor:
	$(FILTEROUTPUT) --port $(ESPPORT) --baud 115200 --elf $(PROGRAM_OUT)
