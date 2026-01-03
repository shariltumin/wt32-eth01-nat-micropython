
# NETWORK_DESIGN.md: IoT Segmentation with WT32-ETH01

This document outlines the architectural advantages of using the **WT32-ETH01** as a dedicated NAT Router to create a segmented IoT "Sandbox."

## 1. The Core Concept: Network Segmentation

In a typical home network, all devices (laptops, phones, and "smart" lightbulbs) sit on the same flat network. If a cheap, unpatched smart plug is compromised, an attacker can move laterally to access your personal computer or NAS.

By using this custom MicroPython firmware, the WT32-ETH01 acts as a **firewall and bridge**, creating a separate subnet for IoT devices.

---

## 2. Advantages of this Design

### 🛡️ Improved Security (Lateral Movement Prevention)

* **Isolation:** IoT devices are often the weakest link in cybersecurity. By placing them behind the WT32-ETH01 NAT, they cannot "see" or scan the devices on your main Ethernet LAN.
* **Controlled Access:** You can define exactly how data flows between the WiFi AP (IoT side) and the Ethernet (Home/WAN side).

### 🚀 Performance & Traffic Management

* **Reduced Broadcast Traffic:** IoT devices are notoriously "chatty," frequently sending mDNS and SSDP broadcast packets. Segmentation keeps this "noise" off your main high-speed network, preventing it from slowing down gaming or video calls.
* **Dedicated Bandwidth:** The WT32-ETH01’s RMII interface handles the uplink to your router at higher speeds than SPI-based alternatives, ensuring that even if multiple IoT devices are communicating, the bottleneck is minimized.

### 🌐 IP Address Management

* **Subnet Cleanliness:** Keep your main router's DHCP table clean. Instead of 50 smart bulbs cluttering your main router’s list, your main router sees only **one** device: the WT32-ETH01.
* **Custom Subnets:** You can run a specific range (e.g., `10.10.6.x`) for IoT devices, making it easy to identify and manage them in your code.

### 🔌 Legacy Compatibility

* **2.4GHz Optimization:** Many modern routers struggle with "Smart Connect" (combined 2.4/5GHz SSIDs). By using the WT32-ETH01 as a dedicated 2.4GHz AP, you provide a stable, compatible environment for older or cheaper IoT chips that refuse to connect to dual-band mesh systems.

---

## 3. Logical Architecture

```text
[ INTERNET ] 
      │
[ MAIN HOME ROUTER ] (192.168.1.1)
      │
      │ (Ethernet Cable - WAN)
      ▼
[ WT32-ETH01 NAT ROUTER ] (WAN IP: 192.168.1.50 | AP IP: 10.10.6.1)
      │
      │ (Private WiFi - 2.4GHz)
      ▼
[ IoT DEVICE 1 ]  [ IoT DEVICE 2 ]  [ IoT DEVICE 3 ]
 (10.10.6.5)       (10.10.6.12)      (10.10.6.20)

```

---

## 4. Why the WT32-ETH01?

We chose the WT32-ETH01 specifically because of its **RMII Ethernet interface**.

* **SPI Ethernet** (like W5500) introduces latency and consumes CPU cycles for every packet sent.
* **RMII Ethernet** uses a dedicated hardware MAC, allowing the ESP32 to focus its processing power on the **NAT translation** and **WiFi management** rather than the bit-banging of the Ethernet protocol itself.

