# ==========================================
#   Unity Project - A Test Framework for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================
UNITY_ROOT = $(abspath ./components/unity)

#We try to detect the OS we are running on, and adjust commands as needed
ifeq ($(OS),Windows_NT)
  ifeq ($(shell uname -s),) # not in a bash-like shell
	CLEANUP = del /F /Q
	MKDIR = mkdir
  else # in a bash-like shell, like msys
	CLEANUP = rm -f
	MKDIR = mkdir -p
  endif
	TARGET_EXTENSION=.exe
else
	CLEANUP = rm -f
	MKDIR = mkdir -p
	TARGET_EXTENSION=.out
endif

C_COMPILER=gcc
ifeq ($(shell uname -s), Darwin)
C_COMPILER=clang
endif


CFLAGS=-std=c99
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wpointer-arith
CFLAGS += -Wcast-align
CFLAGS += -Wwrite-strings
CFLAGS += -Wswitch-default
CFLAGS += -Wunreachable-code
CFLAGS += -Winit-self
CFLAGS += -Wmissing-field-initializers
CFLAGS += -Wno-unknown-pragmas
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wundef
CFLAGS += -Wold-style-definition

TARGET_BASE1=all_tests
TARGET1 = $(TARGET_BASE1)$(TARGET_EXTENSION)
UNITY_FILES=\
  $(UNITY_ROOT)/src/unity.c \
  $(UNITY_ROOT)/extras/fixture/src/unity_fixture.c

SRC_DRVIER_CODE = $(shell find ./src/drivers -type f -name '*.c')
SRC_CONFIG_CODE = $(shell find ./src/config -type f -name '*.c')
SRC_LOGIC_CODE = $(shell find ./src/logic -type f -name '*.c')

TEST_CODE = $(shell find ./test -type f -name '*.c')

INC_DIRS=-I. \
	-I./test \
	-I./include \
	-I$(UNITY_ROOT)/src \
	-I$(UNITY_ROOT)/extras/fixture/src

DEFINES=-DUNITY_INCLUDE_CONFIG_H -DSYSTEM_UNDER_TEST

SYMBOLS=

all: clean default

default:
	$(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(DEFINES) $(SYMBOLS) $(UNITY_FILES) $(TEST_CODE) \
		$(SRC_DRVIER_CODE) \
		$(SRC_CONFIG_CODE) \
		$(SRC_LOGIC_CODE) \
		-o $(TARGET1)
	- ./$(TARGET1) -v

clean:
	$(CLEANUP) $(TARGET1)

ci: CFLAGS += -Werror
ci: default
