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

void Wifi::connectToWifi(const char *ssid, const char *pw, uint32_t authMethod, uint32_t timeout)
{
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        //Failed to connect
    }
    //Connected successfully
}