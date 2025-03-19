#ifndef WIFI_H
#define WIFI_H

#include "pico/cyw43_arch.h"

#include "pico/stdlib.h"

#include "lwip/ip4_addr.h"
#include "lwip/apps/mqtt.h"

#include "FreeRTOS.h"
#include "task.h"
#include "ping.h"

#include "motor.h"
#include "logger.h"

#define MQTT_ADDR   "10.0.2.7" //Home assistant static IP
#define MQTT_ADDR_PORT   1883

#ifndef MQTT_CLIENT_ID
#define MQTT_CLIENT_ID "picow" //Empty
#warning "MQTT_CLIENT_ID not defined! Leaving as PicoW"
#endif

#ifndef MQTT_USER
#define MQTT_USER "" //Empty
#warning "MQTT_USER not defined! Leaving as empty"
#endif

#ifndef MQTT_PASSWORD
#define MQTT_PASSWORD "" //Empty
#warning "MQTT_PASSWORD not defined! Leaving as empty"
#endif

#define MQTT_POS_REQUEST_TOPIC "/blinds/" MQTT_CLIENT_ID "/pos_request"

#ifndef PING_ADDR
//#define PING_ADDR "10.0.2.7"
#define PING_ADDR "8.8.8.8" //Ping google
#endif

class Wifi
{
    public:
        Wifi();
        int init(Motor *mtr); //cyw43_arch_init
        void enableStationMode();
        int connectToWifi(const char *ssid, const char *pw, uint32_t authMethod, uint32_t timeout);
        void initMqtt();
        err_t connectMqtt();

    private:
        Motor *m_mtr;
        mqtt_client_t *m_mqttClient;
        struct mqtt_connect_client_info_t m_ciStruct;

};

extern Wifi wifi;  // Declare a global Wifi object


#endif