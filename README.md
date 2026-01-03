# 🚀 WT32-ETH01 MicroPython NAT Router

## Repository Structure

```text
wt32-eth01-nat-micropython
├── boards
│   └── ESP32_WT32_ETH01         # Custom board definition files
│       ├── board.json
│       ├── board.md
│       ├── manifest.py
│       ├── mpconfigboard.cmake
│       ├── mpconfigboard.h
│       └── sdkconfig.board
├── build-ESP32_WT32_ETH01       # Pre-compiled .bin files for quick flashing
│   ├── firmware.bin
│   └── sdkconfig
├── BUILD.md
├── Doc
│   ├── Ethernet.md
│   ├── NETWORK_DESIGN.md
│   ├── SECURITY.md
│   ├── Troubleshooting.md
│   └── User-Manual.md
├── esp32_common.cmake-LAN-NAT
├── flash.sh                     # Firmware flashing script
├── log                          # Test run log
│   ├── nat_config.py
│   ├── start_nat.md
│   └── sysinfo.md
├── mpb_esp32_lan_nat.sh         # Build scripts
├── mpconfigport.h-LAN-NAT
├── README.md
├── scripts
│   ├── nat_config.py            # Change 'at lease' AP_SSID, AP_PASSWORD
│   └── start_nat.py             # The start script freeze into firmware
└── xmod
    └── lan_router.c             # The C source of nat_router module
```

## Why?

This repository provides everything you need to build and deploy a custom **MicroPython firmware** for the **WT32-ETH01** that supports **NAT (Network Address Translation)**.

While standard MicroPython builds support Ethernet, they often lack the underlying ESP-IDF configuration to route traffic between the WiFi AP and the Wired Ethernet interface. This project bridges that gap.

## 🧠 Why Custom Firmware?

The WT32-ETH01 uses **RMII-based Ethernet** (LAN8720). Unlike SPI-based Ethernet (like the W5500), RMII uses the ESP32's native MAC, offering significantly higher speeds—essential for a router.

To enable NAT, we have to:

1. **Recompile MicroPython** with `IP_FORWARD` and `IP_NAT` enabled in the LWIP stack.
2. Define the specific **RMII pin mapping** for the WT32-ETH01 (MDC/MDIO/Clock).


## ⚡ Features

* **RMII Performance:** Leverages the native MAC for high-speed wired connectivity.
* **NAT Routing:** Seamlessly route data from WiFi clients to the Ethernet WAN.
* **Port Mapping:** Portmapper Support
* **Pre-configured:** Custom board definitions specifically for the WT32-ETH01 hardware.
* **Frozen Module:** Includes helper script to initialize the NAT gateway with a single line of code.

## 🛠 Hardware Comparison: Why RMII?

| Feature | RMII (WT32-ETH01) | SPI (W5500) |
| :-- | :-- | :-- |
| **Throughput** | ~30-50 Mbps | ~5-15 Mbps |
| **CPU Usage** | Low (Hardware MAC) | High (Software SPI) |
| **NAT Suitability** | Excellent | Limited |


## 🚀 Getting Started

### 1. Clone the Repository

```bash
git clone https://github.com/shariltumin/wt32-eth01-nat-micropython.git
cd wt32-eth01-nat-micropython

```

### 2. Build the Firmware

This project uses the official MicroPython build system. Ensure you have the ESP-IDF (v5.x depending on your version) installed.

```bash
# Build for the WT32-ETH01 board
./mpb_esp32_lan_nat.sh
```

### 3. Flash your Device

```bash
# Flash the firmware
./flash.sh
```

## 💻 Usage in MicroPython

Once flashed, setting up the router is simple:

```python
import network
import nat_setup

# Initialize Ethernet (RMII)
lan = network.LAN(mdc=23, mdio=18, power=16, phy_type=network.PHY_LAN8720, phy_addr=1)
lan.active(True)

# Start the NAT Router (WiFi AP -> Ethernet WAN)
nat_setup.start_router(ssid="ESP32-Router", password="password123")

```

or, edit the `nat_config.py` and run `start_nat.py`

```python
import start_nat
```

## ⚠️ Known Issues & Notes

* **Heat:** Running NAT on an ESP32 can generate heat; consider a small heatsink for the ESP32 chip if routing high traffic 24/7.

