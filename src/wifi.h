#ifndef WIFI_H
#define WIFI_H

#include "pico/cyw43_arch.h"

class Wifi
{
    public:
        Wifi();
        int init_wifi();

};

extern Wifi wifi;  // Declare a global Wifi object


#endif