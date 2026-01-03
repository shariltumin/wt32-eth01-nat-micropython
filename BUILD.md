# BUILD.md: Firmware Compilation & Flashing 🛠️

This guide explains how to use the provided automation scripts to build the custom MicroPython firmware with NAT routing support for the **WT32-ETH01**.

## 1. Prerequisites

### Software Requirements

* **Ubuntu/Debian Linux** (Recommended environment).
* **ESP-IDF v5.1.1:** The build script is specifically configured for `esp-idf-551`.
* **Python 3.8+** and standard MicroPython build dependencies (`build-essential`, `libffi-dev`, etc.).

### Hardware Requirements

* **WT32-ETH01 Board.**
* **USB-to-Serial (TTL) Adapter:** (e.g., CP2102 or FT232). The WT32-ETH01 does not have an onboard USB port.
* **Jumper Wire:** To bridge `IO0` to `GND`.

## 2. Environment Setup

The build script expects your ESP-IDF to be located at `~/disk/esp/esp-idf-551`. If yours is in a different location, update the `IDF_PATH` variable in the script before running.

```bash
# Ensure the script is executable
chmod +x mpb_esp32_lan_nat.sh

```

---

## 3. The Automated Build Process

Run the build script from the root of the MicroPython port directory:

```bash
./mpb_esp32_lan_nat.sh

```

### What the script does:

1. **Exports Paths:** Sets up the ESP-IDF environment variables.
2. **Swaps Configurations:** It automatically replaces the standard `esp32_common.cmake` and `mpconfigport.h` with the **LAN-NAT** versions required for the NAT engine and RMII Ethernet support.
3. **Compiles:** Runs the `make` command specifically for the `ESP32_WT32_ETH01` board definition.
4. **Cleans Up:** After the build, it restores the original MicroPython config files so your repository remains "clean."

The resulting firmware will be located in:
`build-ESP32_WT32_ETH01/firmware.bin`

## 4. Flashing the Hardware ⚡

### Step 1: Verify the Binary:

```bash
strings firmware.bin | grep nat_router
```

Flashing the WT32-ETH01 requires manual intervention to enter **Bootloader Mode**.

### Step 2: Wiring

Connect your USB-to-Serial adapter to the WT32-ETH01 as follows:

* **TX** (Adapter) → **RX0** (WT32)
* **RX** (Adapter) → **TX0** (WT32)
* **5V** → **5V**
* **GND** → **GND**

### Step 3: Bootloader Mode (Short IO0 to GND)

1. **Short Pin 0 (IO0) to GND** using a jumper wire.
2. While keeping them shorted, plug the USB adapter into your PC (or press the Reset button if you have one).
3. The board is now in **Flash Mode**. You can remove the jumper after the flashing starts.

### Step 4: Flash the Firmware

Use `esptool.py` to write the binary:

```bash
esptool.py --chip esp32 --port /dev/ttyUSB0 erase_flash
esptool.py --chip esp32 --port /dev/ttyUSB0 write_flash -z 0x1000 build-ESP32_WT32_ETH01/firmware.bin

```

Or just run the `flash.sh` script.

## 5. Build Script Summary

The script ensures that the `xmod/nat_router.c` module is included in the build by swapping the `cmake` files. This is a crucial step that enables the `import nat_router` functionality in MicroPython.

> **Note:** Always remember to disconnect the `IO0` to `GND` short after flashing and reboot the device to start the newly installed firmware.


