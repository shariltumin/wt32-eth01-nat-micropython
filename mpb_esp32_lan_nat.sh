#!/bin/bash

export PATH="$HOME/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"

echo "=========================================================="
echo "Setting up for esp32 build"
echo "Using esp-idf-551"
export IDF_PATH="$HOME/disk/esp/esp-idf-551"
echo "MCU type esp32"
export IDF_TARGET="esp32"
source $IDF_PATH/export.sh

# clean-up last build
rm -rf build-ESP32_WT32_ETH01

# Make nat router v/MP network modules -- xmod/nat_router.c
# included in esp32_common.cmake-LAN-NAT

# LAN router
cp esp32_common.cmake-LAN-NAT esp32_common.cmake
cp mpconfigport.h-LAN-NAT mpconfigport.h
# ESP32
# make V=1 BOARD=ESP32_KAKI5_LAN
make BOARD=ESP32_WT32_ETH01
# restore files
cp esp32_common.cmake-ORIG esp32_common.cmake
cp mpconfigport.h-ORIG mpconfigport.h

echo "=========================================================="

# connect PIN0 to GND for bootloader


