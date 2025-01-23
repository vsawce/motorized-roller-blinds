#ifndef WIFI_H
#define WIFI_H

#include "pico/cyw43_arch.h"

#include "pico/stdlib.h"

#include "lwip/ip4_addr.h"

#include "FreeRTOS.h"
#include "task.h"
#include "ping.h"

class Wifi
{
    public:
        Wifi();
        int init(); //cyw43_arch_init

};

extern Wifi wifi;  // Declare a global Wifi object


#endif