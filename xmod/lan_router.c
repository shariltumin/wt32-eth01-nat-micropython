/*------------------------------------------------------------------------------
  
   MIT License
   
   Copyright (c) 2025 shariltumin
   
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.

-------------------------------------------------------------------------------*/
#include "py/runtime.h"
#include "py/obj.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "lwip/lwip_napt.h"
#include "lwip/ip4_napt.h"
#include "lwip/dhcp.h"
#include <stdio.h>
#include "dhcpserver/dhcpserver.h"
#include "dhcpserver/dhcpserver_options.h"

#define DEFAULT_DNS "8.8.8.8"

// Define the wlan_if_obj_t structure (if needed for MicroPython internal use)
typedef struct _wlan_if_obj_t {
    mp_obj_base_t base;
    esp_netif_t *netif;
} wlan_if_obj_t;

// Define the nat_router_obj_t structure
typedef struct _nat_router_obj_t {
    mp_obj_base_t base;
    mp_obj_t wlan_ap;
    mp_obj_t eth_lan;
} nat_router_obj_t;

// Function to initialize the NAT router
static mp_obj_t nat_router_init(mp_obj_t self_in, mp_obj_t wlan_ap_in, mp_obj_t eth_lan_in) {
    nat_router_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->wlan_ap = wlan_ap_in;
    self->eth_lan = eth_lan_in;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(nat_router_init_obj, nat_router_init);

// Function to start the NAT router
static mp_obj_t nat_router_start(mp_obj_t self_in) {
    (void)self_in;

    // Get the network interface handles
    esp_netif_t *lan_netif = esp_netif_get_handle_from_ifkey("ETH_DEF");
    esp_netif_t *ap_netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");

    if (lan_netif == NULL) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("LAN network interface not found. Ensure Ethernet is initialized."));
    }
    if (ap_netif == NULL) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("AP network interface not found. Ensure Wi-Fi AP is initialized."));
    }
    mp_printf(MP_PYTHON_PRINTER, "Stop DHCP server on AP first.\n");
    // Stop DHCP server first
    esp_netif_dhcps_stop(ap_netif);

    // Enable DNS (offer) for dhcp server
    dhcps_offer_t dhcps_dns_value = OFFER_DNS;
    esp_netif_dhcps_option(ap_netif,ESP_NETIF_OP_SET, ESP_NETIF_DOMAIN_NAME_SERVER, &dhcps_dns_value, sizeof(dhcps_dns_value));

    // Get DNS from LAN interface and set it for AP
    mp_printf(MP_PYTHON_PRINTER, "NAT Router: Start DNS ");
    esp_netif_dns_info_t lan_dns;
    if (esp_netif_get_dns_info(lan_netif, ESP_NETIF_DNS_MAIN, &lan_dns) == ESP_OK) {
        esp_netif_set_dns_info(ap_netif, ESP_NETIF_DNS_MAIN, &lan_dns);
	mp_printf(MP_PYTHON_PRINTER, "on AP from LAN.\n");
    } else {
        // Fallback to public DNS
        esp_netif_dns_info_t dns_info;
        dns_info.ip.u_addr.ip4.addr = esp_ip4addr_aton(DEFAULT_DNS);
        dns_info.ip.type = IPADDR_TYPE_V4;
        esp_netif_set_dns_info(ap_netif, ESP_NETIF_DNS_MAIN, &dns_info);
	mp_printf(MP_PYTHON_PRINTER, "on AP from 8.8.8.8\n");
    }

    mp_printf(MP_PYTHON_PRINTER, "NAT Router: Enabling NAPT on AP interface.\n");
    // Enable NAPT on the AP (internal) interface
    esp_netif_napt_enable(ap_netif);

    mp_printf(MP_PYTHON_PRINTER, "NAT Router: Start DHCP server on AP.\n");
    esp_netif_dhcps_start(ap_netif);

    mp_printf(MP_PYTHON_PRINTER, "NAT Router: Setting default network interface to LAN.\n");


    // Set the default interface to LAN
    esp_netif_set_default_netif(lan_netif);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(nat_router_start_obj, nat_router_start);

// Function to add a port mapping
static mp_obj_t nat_router_add_portmap(mp_obj_t self_in, mp_obj_t args_obj) {
    (void)self_in;
    if (!mp_obj_is_type(args_obj, &mp_type_tuple) && !mp_obj_is_type(args_obj, &mp_type_list)) {
        mp_raise_TypeError(MP_ERROR_TEXT("Expected a tuple or list"));
    }
    size_t n_args = mp_obj_get_int(mp_obj_len(args_obj));
    if (n_args < 5) {
        mp_raise_ValueError(MP_ERROR_TEXT("Expected 5 arguments: proto, maddr, mport, daddr, dport"));
    }
    mp_obj_t *args;
    mp_obj_get_array(args_obj, &n_args, &args);
    uint32_t proto = mp_obj_get_int(args[0]);
    const char *maddr_str = mp_obj_str_get_str(args[1]);
    uint16_t mport = mp_obj_get_int(args[2]);
    const char *daddr_str = mp_obj_str_get_str(args[3]);
    uint16_t dport = mp_obj_get_int(args[4]);

    ip4_addr_t maddr_ip, daddr_ip;
    if (!ip4addr_aton(maddr_str, &maddr_ip)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid master IP address"));
    }
    if (!ip4addr_aton(daddr_str, &daddr_ip)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid destination IP address"));
    }

    mp_printf(MP_PYTHON_PRINTER, "NAT Router: Adding portmap - Proto: %d, Master IP: %s:%d, Dest IP: %s:%d\n",
              proto, maddr_str, mport, daddr_str, dport);
    int ret = ip_portmap_add(proto, maddr_ip.addr, mport, daddr_ip.addr, dport);
    if (ret == 0) {
        mp_printf(MP_PYTHON_PRINTER, "NAT Router: Portmap added successfully.\n");
        return mp_const_true;
    } else {
        mp_printf(MP_PYTHON_PRINTER, "NAT Router: Failed to add portmap, error code: %d.\n", ret);
        return mp_const_false;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_2(nat_router_add_portmap_obj, nat_router_add_portmap);

// Function to remove a port mapping
static mp_obj_t nat_router_remove_portmap(mp_obj_t self_in, mp_obj_t args_obj) {
    (void)self_in;
    if (!mp_obj_is_type(args_obj, &mp_type_tuple) && !mp_obj_is_type(args_obj, &mp_type_list)) {
        mp_raise_TypeError(MP_ERROR_TEXT("Expected a tuple or list"));
    }
    size_t n_args = mp_obj_get_int(mp_obj_len(args_obj));
    if (n_args < 3) {
        mp_raise_ValueError(MP_ERROR_TEXT("Expected 3 arguments: proto, mport, dport (dport is not used in remove, but kept for consistency)"));
    }
    mp_obj_t *args;
    mp_obj_get_array(args_obj, &n_args, &args);
    uint32_t proto = mp_obj_get_int(args[0]);
    uint16_t mport = mp_obj_get_int(args[1]);

    mp_printf(MP_PYTHON_PRINTER, "NAT Router: Removing portmap - Proto: %d, Master Port: %d\n", proto, mport);
    int ret = ip_portmap_remove(proto, mport);
    if (ret == 0) {
        mp_printf(MP_PYTHON_PRINTER, "NAT Router: Portmap removed successfully.\n");
        return mp_const_true;
    } else {
        mp_printf(MP_PYTHON_PRINTER, "NAT Router: Failed to remove portmap, error code: %d.\n", ret);
        return mp_const_false;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_2(nat_router_remove_portmap_obj, nat_router_remove_portmap);


// Define the methods exposed to MicroPython
static const mp_rom_map_elem_t nat_router_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&nat_router_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&nat_router_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_add_portmap), MP_ROM_PTR(&nat_router_add_portmap_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove_portmap), MP_ROM_PTR(&nat_router_remove_portmap_obj) },
};
static MP_DEFINE_CONST_DICT(nat_router_locals_dict, nat_router_locals_dict_table);

// Function to create a new NAT router object
static mp_obj_t nat_router_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    nat_router_obj_t *self = mp_obj_malloc(nat_router_obj_t, type);
    return MP_OBJ_FROM_PTR(self);
}

// This defines the type object.
MP_DEFINE_CONST_OBJ_TYPE(
    nat_router_type,
    MP_QSTR_nat_router,
    MP_TYPE_FLAG_NONE,
    make_new, nat_router_make_new,
    locals_dict, &nat_router_locals_dict
);

// Define the module globals
static const mp_rom_map_elem_t nat_router_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_nat_router) },
    { MP_ROM_QSTR(MP_QSTR_NATRouter), MP_ROM_PTR(&nat_router_type) },
    { MP_ROM_QSTR(MP_QSTR_PROTO_TCP), MP_ROM_INT(6) },
    { MP_ROM_QSTR(MP_QSTR_PROTO_UDP), MP_ROM_INT(17) },
    { MP_ROM_QSTR(MP_QSTR_PROTO_ICMP), MP_ROM_INT(1) },
};
static MP_DEFINE_CONST_DICT(nat_router_globals, nat_router_globals_table);

// Define the module
const mp_obj_module_t nat_router_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&nat_router_globals,
};

// Register the module
MP_REGISTER_MODULE(MP_QSTR_nat_router, nat_router_module);

