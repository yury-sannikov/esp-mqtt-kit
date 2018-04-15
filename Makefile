PROGRAM=aws_iot
PROGRAM_SRC_DIR=./src
EXTRA_COMPONENTS = \
	extras/paho_mqtt_c \
	extras/mbedtls

include ./rtos/common.mk
