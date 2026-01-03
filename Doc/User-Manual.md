### 📘 User-Manual.md

# User Manual: WT32-ETH01 NAT Router 🛠️

This guide will walk you through configuring and deploying the NAT Router on your WT32-ETH01 using our custom MicroPython firmware.

## 1. Prerequisites

* **Firmware:** You must have the custom firmware from this repo flashed (standard MicroPython does not support the `nat_router` module).
* **Hardware:** WT32-ETH01 development board.
* **Tools:** Thonny IDE, `mpremote`, or `ampy` to upload files.

---

## 2. Configuration Setup

Instead of editing the boot script, all user changes should be made in `nat_config.py`.

1. Open `nat_config.py`.
2. Edit the `AP_SSID` and `AP_PASSWORD` to your desired WiFi credentials.
3. Adjust `AP_IFCONFIG` if you need a different subnet for your wireless clients.
4. Upload `nat_config.py` to the root directory of your ESP32.

---

## 3. Understanding the Startup Process

The `start_nat.py` (or your start script) performs the following sequence to ensure a stable connection:

1. **Hardware Initialization:** The LAN8720 RMII Ethernet PHY is initialized. The WT32-ETH01 uses specific pins (`MDC: 23`, `MDIO: 18`, `Power: 16`, `Ref_Clk: 0`) that are hardcoded for stability.
2. **Ethernet Handshake:** The system waits for a DHCP lease from your WAN/Wired source.
3. **Access Point (AP) Start:** The WiFi radio enters AP mode using your config settings.
4. **NAT Routing:** The `nat_router` module links the `AP` interface to the `LAN` interface, enabling packet forwarding.

---

## 4. Deployment

Once your files are uploaded, your directory structure on the device should look like this:

```text
/
├── boot.py         # Standard MicroPython boot
├── main.py         # Your start script (logic)
├── nat_config.py   # Your settings (SSID/Pass)
└── start_nat.py    # Import NAT binary/module and start router

```

## 5. Troubleshooting

* **No Internet on WiFi:** Check if the LAN interface received an IP from your router. Look for `LAN active with IP: 192.168.x.x` in the console.
* **Authentication Error:** Ensure `authmode` is set to `3` (WPA2-PSK) in the config if you are using a modern device to connect.
* **Firmware Issues:** If `import nat_router` fails, ensure you have flashed the **custom** build provided in this repository, not the generic ESP32 firmware.

---

### 🏗️ Start Script 

```python
import esp
import machine, network
from time import sleep
import nat_config # Import the user settings

# Debug settings
esp.osdebug(esp.LOG_INFO if nat_config.DEBUG_MODE else None)

# 1. Initialize RMII Ethernet (Fixed Pins for WT32-ETH01)
lan = network.LAN(mdc=machine.Pin(23), 
                mdio=machine.Pin(18), 
                power=machine.Pin(16),
                phy_type=network.PHY_LAN8720, phy_addr=1,
                ref_clk_mode=machine.Pin.IN, 
                ref_clk=machine.Pin(0))
lan.active(True)

print("Waiting for Ethernet...")
sleep(nat_config.BOOT_DELAY)
print("LAN active with IP:", lan.ifconfig()[0])

# 2. Initialize WiFi Access Point
ap = network.WLAN(network.AP_IF)
ap.active(True)
ap.config(essid=nat_config.AP_SSID, password=nat_config.AP_PASSWORD, authmode=3)
ap.ifconfig(nat_config.AP_IFCONFIG)

sleep(nat_config.BOOT_DELAY)
print("AP mode active with IP:", ap.ifconfig()[0])

# 3. Start NAT Routing
import nat_router
print('Starting NAT Engine...')
nat = nat_router.NATRouter()
nat.init(ap, lan)
nat.start()

print('--- ROUTER READY ---')
print('WAN (LAN):', lan.ifconfig())
print('LAN (AP):', ap.ifconfig())

```
