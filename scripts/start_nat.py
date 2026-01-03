
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


