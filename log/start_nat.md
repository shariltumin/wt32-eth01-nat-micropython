The `start_nat.py` script is included in the firmware as a frozen module. To start your router, you only need the `nat_config.py` file.

```bash
$ mpr mount .
Local directory . is mounted at /remote
Connected to MicroPython at /dev/ttyUSB0
Use Ctrl-] or Ctrl-x to exit this shell
>
MicroPython v1.27.0-kaki5 on 2026-01-03; WT32-ETH01 LAN+NAT (KAKI5) with ESP32
>>> import start_nat
Waiting for Ethernet...
LAN active with IP: 192.168.5.23
AP mode active with IP: 10.10.6.1
Starting NAT Engine...
Stop DHCP server on AP first.
NAT Router: Start DNS on AP from LAN.
NAT Router: Enabling NAPT on AP interface.
NAT Router: Start DHCP server on AP.
NAT Router: Setting default network interface to LAN.
--- ROUTER READY ---
WAN (LAN): ('192.168.5.23', '255.255.252.0', '192.168.4.1', '192.168.4.1')
LAN (AP): ('10.10.6.1', '255.255.255.0', '10.10.6.1', '192.168.4.1')
>>> 
```

If you forget to connect an Ethernet cable to your board, you will receive the following output:

```bash
>>> import start_nat
Waiting for Ethernet...
LAN active with IP: 0.0.0.0
AP mode active with IP: 10.10.6.1
Starting NAT Engine...
Stop DHCP server on AP first.
NAT Router: Start DNS on AP from LAN.
NAT Router: Enabling NAPT on AP interface.
NAT Router: Start DHCP server on AP.
NAT Router: Setting default network interface to LAN.
--- ROUTER READY ---
WAN (LAN): ('0.0.0.0', '0.0.0.0', '0.0.0.0', '0.0.0.0')
LAN (AP): ('10.10.6.1', '255.255.255.0', '10.10.6.1', '1.1.1.1')
```

