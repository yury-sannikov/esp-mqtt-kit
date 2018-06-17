#!/usr/bin/env bash
export FLASH_SIZE=8
export ESPPORT=/dev/cu.wchusbserial1410
make -C . all
# make -C . flash
# make -C . monitor