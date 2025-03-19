#include "wifi.h"

void mqtt_incoming_data_cb(void *arg, const uint8_t *data, uint16_t len, uint8_t flags) {
    log_send(LogType::DEBUG, "Incoming publish payload with length %d, flags %u", len, (unsigned int)flags);

    if (flags & MQTT_DATA_FLAG_LAST) {
        /* Last fragment of payload received (or whole part if payload fits receive buffer
            See MQTT_VAR_HEADER_BUFFER_LEN)  */
        
        log_send(LogType::DEBUG, "flags & MQTT_DATA_FLAG_LAST");

        //String copy buffer. +1 for null char
        char data_cpy[len+1];
        memcpy(data_cpy, data, len); //Set all members to null char
        data_cpy[len] = '\0'; //Manually append null char
        log_send(LogType::DEBUG, "Content: %s", data_cpy);
    }
    else {
        /* Handle fragmented payload, store in buffer, write to file or whatever */
    }
}

void mqtt_sub_request_cb(void *arg, err_t result) {
    log_send(LogType::STANDARD, "Subscribe result: %d\n", result);
}

void mqtt_incoming_publish_cb(void *arg, const char *topic, uint32_t total_len) {
    log_send(LogType::DEBUG, "Incoming publish (topic: %s, length %u)\n", topic, (unsigned int)total_len);
}

//cpp-only function
void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    err_t err;
    if (status == MQTT_CONNECT_ACCEPTED) {
        log_send(LogType::STANDARD, "mqtt_connection_cb: Successfully connected\n");

        mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, arg);

        log_send(LogType::STANDARD, "mqtt_connection_cb: mqtt_set_inpub_callback done\n");

        // Subscribe to topic MQTT_POSITION_TOPIC with QoS 0
        err = mqtt_subscribe(client, MQTT_POSITION_TOPIC, 0, mqtt_sub_request_cb, arg);

        if (err != ERR_OK) {
            log_send(LogType::STANDARD, "mqtt_subscribe return: %d\n", err);
        }
    }
    else {
        log_send(LogType::STANDARD, "mqtt_connection_cb: Disconnected, reason: %d\n", status);
        //   Try to reconnect?
    }
}

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

void Wifi::initMqtt()
{
    m_mqttClient = mqtt_client_new();

    /* Setup an empty client info structure */
    memset(&m_ciStruct, 0, sizeof(m_ciStruct));

    /* Minimal amount of information required is client identifier, so set it here */
    m_ciStruct.client_id = MQTT_CLIENT_ID;
    m_ciStruct.client_user = MQTT_USER;
    m_ciStruct.client_pass = MQTT_PASSWORD;
    m_ciStruct.keep_alive = 0;
    m_ciStruct.will_topic = NULL;
    m_ciStruct.will_msg = NULL;
    m_ciStruct.will_retain = 0;
    m_ciStruct.will_qos = 0;
}

void Wifi::enableStationMode()
{
    cyw43_arch_enable_sta_mode();
}

int Wifi::connectToWifi(const char *ssid, const char *pw, uint32_t authMethod, uint32_t timeout)
{
    return cyw43_arch_wifi_connect_timeout_ms(ssid, pw, authMethod, 30000);
}

err_t Wifi::connectMqtt()
{    
    err_t err;

    ip_addr_t mqtt_ip;
    ip4addr_aton(MQTT_ADDR, &mqtt_ip);

    cyw43_arch_lwip_begin();
    err = mqtt_client_connect(m_mqttClient, &mqtt_ip, MQTT_ADDR_PORT, mqtt_connection_cb, 0, &m_ciStruct);
    cyw43_arch_lwip_end();

    /* For now just print the result code if something goes wrong*/
    if (err != ERR_OK) {
        log_send(LogType::STANDARD, "mqtt_connect return %d\n", err);
    }

    return err;
}