# -freeze("$(PORT_DIR)/modules")
# freeze("$(PORT_DIR)/modules", ("_boot.py", "espnow.py", "flashbdev.py", "inisetup.py"))
freeze("$(PORT_DIR)/modules", ("_boot.py", "flashbdev.py", "inisetup.py", "machine.py"))
# -include("$(MPY_DIR)/extmod/asyncio")

# board specific modules
# -freeze("modules", 'app.py')

# Require some micropython-lib modules.
# -require("dht")
# -require("ds18x20")
# -require("neopixel")
# -require("onewire")
# -require("umqtt.robust")
# -require("umqtt.simple")
# -require("upysh")
require("mip")
require("ntptime")
# -require("urequests")
# -require("webrepl")

# put your extra modules below
freeze('$(MPY_DIR)/extra', 'start_nat.py')
