#include "wifi.h"

Wifi wifi;

Wifi::Wifi()
{
    //Empty for now...
}

int Wifi::init_wifi()
{
    if (cyw43_arch_init()) {
        //printf("Failed to initialize the CYW43 architecture.\n");
        return -1;
    }
    return 0;
}