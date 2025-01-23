#include "wifi.h"

Wifi wifi;

Wifi::Wifi()
{
    //Empty for now...
}

int Wifi::init()
{
    if (cyw43_arch_init()) {
        //printf("Failed to initialize the CYW43 architecture.\n");
        return -1;
    }
    return 0;
}

void Wifi::enableStationMode()
{
    cyw43_arch_enable_sta_mode();
}

int Wifi::connectToWifi(const char *ssid, const char *pw, uint32_t authMethod, uint32_t timeout)
{
    return cyw43_arch_wifi_connect_timeout_ms(ssid, pw, authMethod, 30000);
}