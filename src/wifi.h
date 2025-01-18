#ifndef WIFI_H
#define WIFI_H

#include "pico/cyw43_arch.h"

class Wifi
{
    public:
        Wifi();
        int init(); //cyw43_arch_init

};

extern Wifi wifi;  // Declare a global Wifi object


#endif