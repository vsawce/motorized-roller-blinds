#ifndef WIFI_H
#define WIFI_H

#include "pico/cyw43_arch.h"

#include "pico/stdlib.h"

#include "lwip/ip4_addr.h"

#include "FreeRTOS.h"
#include "task.h"
#include "ping.h"

#ifndef PING_ADDR
#define PING_ADDR "8.8.8.8" //Ping google
#endif

class Wifi
{
    public:
        Wifi();
        int init(); //cyw43_arch_init
        void enableStationMode();
        int connectToWifi(const char *ssid, const char *pw, uint32_t authMethod, uint32_t timeout);

};

extern Wifi wifi;  // Declare a global Wifi object


#endif