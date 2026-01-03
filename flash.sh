#!/bin/bash

# Configuration
FIRMWARE_BIN="build-ESP32_WT32_ETH01/firmware.bin"
BAUD_RATE=921600
FLASH_OFFSET=0x1000

echo "=========================================================="
echo "WT32-ETH01 Flash Utility"
echo "=========================================================="

# 1. Check if firmware exists
if [ ! -f "$FIRMWARE_BIN" ]; then
    echo "❌ Error: $FIRMWARE_BIN not found!"
    echo "Please run your build script first."
    exit 1
fi

# 2. Auto-detect Serial Port (Linux focus)
SERIAL_PORT=$(ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null | head -n 1)

if [ -z "$SERIAL_PORT" ]; then
    echo "❌ Error: No USB-Serial adapter detected."
    echo "Please connect your WT32-ETH01 via your TTL adapter."
    exit 1
fi

echo "✅ Detected Port: $SERIAL_PORT"
echo "⚠️  REMINDER: Short PIN0 to GND and cycle power/reset NOW."
echo "Waiting 5 seconds for bootloader mode..."
sleep 5

# 3. Execution
echo "----------------------------------------------------------"
echo "Step 1: Erasing Flash..."
esptool.py --chip esp32 --port "$SERIAL_PORT" erase_flash

echo "Step 2: Writing Optimized Firmware..."
esptool.py --chip esp32 --port "$SERIAL_PORT" --baud $BAUD_RATE write_flash -z $FLASH_OFFSET "$FIRMWARE_BIN"

echo "----------------------------------------------------------"
echo "✅ Success!"
echo "⚠️  IMPORTANT: Remove the PIN0-to-GND jumper and reset the board."
echo "Your NAT Router is now initializing."


