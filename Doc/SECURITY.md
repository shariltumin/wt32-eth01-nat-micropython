# SECURITY.md: NAT & Firewall Architecture 🛡️

This document outlines the security features of the **WT32-ETH01 NAT Router** and how it protects your IoT ecosystem.

## 1. Stateful Packet Inspection (SPI)

By default, the NAT engine acts as a basic stateful firewall.

* **Inbound Protection:** All unsolicited traffic originating from the Ethernet (WAN) side is dropped.
* **Stateful Tracking:** The router only allows incoming packets if they are part of a connection already initiated by an internal WiFi (IoT) device.

---

## 2. Port Mapping (Portmapper Support)

The `nat_router` module supports **Port Mapping**, allowing you to expose specific internal IoT services to your main network without compromising the entire subnet.

### Use Case: Accessing an IoT Web Camera

If you have a camera at `10.10.6.5:80`, you can map it to the WT32-ETH01's WAN IP at port `8080`.

```python
# Example of manual port mapping
nat.add_portmap(nat_router.PROTO_TCP, '192.168.1.50', 8080, '10.10.6.5', 80)

```

* **Advantage:** You can reach the camera from your PC on the main network via `192.168.1.50:8080`, but the camera itself remains isolated from your PC's sensitive data.

---

## 3. Hardened Firmware (Minimal Attack Surface)

This firmware is intentionally "stripped" to reduce the attack surface for local and remote exploits.

* **No `help()` Module:** Removing interactive help prevents an unauthorized user with serial access from easily discovering available modules, methods, or system variables.
* **Stripped Symbols:** Most function names and docstrings are removed from the binary, hindering reverse engineering efforts.
* **No Bluetooth:** By disabling the Bluetooth stack in `sdkconfig.board`, we eliminate an entire wireless attack vector (BlueBorne, etc.) that typically exists on standard ESP32 builds.

---

## 4. IoT Sandbox Isolation

The primary security goal is **Micro-Segmentation**.

1. If an IoT device is compromised via a cloud vulnerability, the attacker is "trapped" in the `10.10.6.x` subnet.
2. The NAT prevents the attacker from scanning your primary home network (`192.168.1.x`) for laptops, NAS drives, or security cameras.

---

## 5. Security Best Practices

* **Physical Access:** Since the REPL provides root-level access, the WT32-ETH01 should be placed in a physically secure location.
* **Password Management:** Always change the default SSID and Password in `nat_config.py` before deployment.
* **Firmware Updates:** Regularly re-build and flash your firmware to incorporate the latest ESP-IDF security patches for the LWIP stack.

