
# Troubleshooting.md: NAT Router & Connectivity

This guide covers common hurdles when deploying the WT32-ETH01 as an IoT Gateway.

## 1. Connectivity Issues

### No Internet on WiFi Clients

* **Check WAN Link:** Ensure the Ethernet cable is securely plugged in and the upstream router has assigned an IP. Check the serial console for: `LAN active with IP: 192.168.x.x`.
* **DNS Settings:** If you can ping `8.8.8.8` from a client but cannot browse the web, your DNS is likely the issue. Ensure `AP_IFCONFIG` in `nat_config.py` has a valid DNS (e.g., `1.1.1.1` or `8.8.8.8`).
* **NAT Module Not Started:** Ensure you see `Starting NAT Engine...` in your boot logs. If the module isn't initialized, the ESP32 acts as a standard AP with no "path" to the Ethernet port.

### WiFi Devices Cannot Connect

* **Authentication Mode:** We use `authmode=3` (WPA2-PSK). If your IoT device is extremely old, it might require WPA (mode 2), though this is not recommended for security.
* **IP Conflict:** Ensure the subnet defined in `nat_config.py` (e.g., `10.10.6.x`) is **different** from your main home network subnet (e.g., `192.168.1.x`). NAT cannot route between two identical subnets.

---

## 2. Advanced Performance Tuning

### High Latency / Packet Loss

* **Power Supply:** The WT32-ETH01 is power-hungry when both WiFi and Ethernet are active. Ensure you are providing at least **5V/500mA** of clean power.
* **Heat Dissipation:** Processing NAT tables at high speeds can cause the ESP32 to throttle. If the device feels hot to the touch, add a small adhesive heatsink.

---

# 🛰️ Why MQTT Works Behind NAT

One of the biggest advantages of using an **MQTT** architecture with this NAT router is that it solves the "Reachability Problem."

### 1. Outbound Connections (Client-to-Broker)

NAT routers generally allow all outbound traffic but block unsolicited inbound traffic. Because MQTT relies on the **IoT Device (Client)** initiating a connection to the **Broker** (usually on your main network or the cloud), the NAT router creates a "stateful" entry.

* The ESP32 remembers that your Smart Plug asked to talk to the Broker.
* When the Broker sends a message back, the NAT router knows exactly which IoT device to send it to.

### 2. The "Keep-Alive" Advantage

MQTT uses a **Keep-Alive** (Heartbeat) mechanism. By sending a tiny packet every 60 seconds, the IoT device keeps the NAT "hole" open. This ensures the connection stays active 24/7 without you needing to configure "Port Forwarding" for every single lightbulb.

### 3. Asynchronous Communication

Since the MQTT Broker acts as a post office, your IoT devices don't need to know anything about the outside world. They only need to know the IP of the Broker. This makes the **Segmentation** we designed in `NETWORK_DESIGN.md` extremely robust—your devices remain isolated and secure, yet perfectly responsive.

