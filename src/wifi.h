#ifndef WIFI_H
#define WIFI_H

#include "pico/cyw43_arch.h"

#include "pico/stdlib.h"

#include "lwip/ip4_addr.h"

#include "FreeRTOS.h"
#include "task.h"
#include "ping.h"

#define MQTT_ADDR   "10.0.2.7" //Home assistant static IP
#define MQTT_ADDR_PORT   1883

#ifndef MQTT_USER
#define MQTT_USER "" //Empty
#warning "MQTT_USER not defined! Leaving as empty"
#endif

#ifndef MQTT_PASSWORD
#define MQTT_PASSWORD "" //Empty
#warning "MQTT_PASSWORD not defined! Leaving as empty"
#endif

#ifndef PING_ADDR
//#define PING_ADDR "10.0.2.7"
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