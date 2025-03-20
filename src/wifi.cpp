#include "wifi.h"

static Motor *mtr_;

uint8_t json_get_percent_val(const char *data, uint16_t data_len, const char *key, uint16_t key_len)
{
    //255 is invalid return value
    uint8_t percent_val = 255;

    //  Zero length
    if (!data_len || !key_len) {
        log_send(LogType::DEBUG, "json_get_percent_val zero length! data_len=%d, key_len=%u", data_len, key_len);
        return percent_val;
    }
    //key is bigger than data
    if (key_len > data_len) {
        log_send(LogType::DEBUG, "json_get_percent_val key_len cannot be bigger than data_len! data_len=%d, key_len=%u", data_len, key_len);
        return percent_val;
    }
    //Null string
    if (data == NULL || key == NULL) {
        log_send(LogType::DEBUG, "json_get_percent_val null string!");
        return percent_val;
    }

    char data_cpy[data_len+1];
    memcpy(data_cpy, data, data_len); //Set all members to null char
    data_cpy[data_len] = '\0'; //Manually append null char
    
    uint16_t i = 0; //Index incrementing through 
    uint16_t j = 0; //To mark first index of key or value
    uint8_t foundFirstQuote = 0; //If 1, we found first quote of a key or value
    uint8_t expectKey = 1; //If 0, expect value and not key
    uint8_t keyFound = 0;
    uint8_t firstIdxSet = 0; //If 1, then we have marked a valid j

    // // Bit 0 = Found first quote (If 1, we found first quote of a key or value)
    // // Bit 1 = Expect key (//If 0, expect value and not key)
    // // Bit 2 = Key was hit (If 1, key was hit!)
    // uint8_t flag = 0b011;

    // While not json ending bracket or null character
    // Parse sequentially through array
    // ex: {"pos":"50%"}
    // 0   1   2   3   4   5   6   7   8   9   10  11  12
    // {   "   p   o   s   "   :   "   5   0   %   "   }

    while (data_cpy[i] != '}' && data_cpy[i] != '\0') {
        if (data_cpy[i] == '\"') {
            if (!foundFirstQuote) {     //If flag indicates we haven't found first quote yet
                foundFirstQuote = 1;    //Found the first quote
            }
            else {  //Second/closing quote
                //Found quote so end key/value parse, subtract j from i
                if (i-j) { //Non empty key or value
                    //char keyval[i-j];
                    //memcpy(keyval, &data_cpy[j], i-j);
                    if (expectKey) {
                        char readKey[i-j+1]; //Minus 1 to delete %
                        memcpy(readKey, &data_cpy[j], i-j);
                        readKey[i-j] = '\0';
                        if (!strncmp(readKey, key, key_len)) { //Are read key and target key the same?
                            log_send(LogType::DEBUG, "json_get_percent_val found key: %s", readKey);
                            keyFound = 1; //Flag val as the next one is to be returned
                        }
                    }
                    else { //expect value
                        if (keyFound) { //We found the value to return!
                            char readVal[i-j]; //Minus 1 to delete %
                            memcpy(readVal, &data_cpy[j], i-j-1);
                            readVal[i-j-1] = '\0';
                            percent_val = strtoul(readVal, NULL, 10);
                            log_send(LogType::DEBUG, "json_get_percent_val found val: %s", readVal);
                            break;
                        }
                    }
                }

                expectKey = !expectKey; //Flip expectation on second/closing quote
                firstIdxSet = 0; //Reset
                foundFirstQuote = 0; //Reset
            }
        }
        else {
            if (foundFirstQuote && !firstIdxSet) {
                j = i; //Mark first index of key or value
                firstIdxSet = 1;
            }
        }
        i++; //Move to next character
    }

    return percent_val;
}

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

        MotorCommandMessage cmd;
        cmd.mc = MotorCommand::ROTATE_TO_PERCENT;
        cmd.pos = json_get_percent_val(data_cpy, len, MQTT_POS_JSON_KEY, MQTT_POS_JSON_KEY_LEN);
        log_send(LogType::DEBUG, "json_get_percent_val sending MotorCommandMessage to queue with pos=%u...", cmd.pos);
        xQueueSend(mtr_->getCommandQueue(), &cmd, pdMS_TO_TICKS(CMD_TIMEOUT_MS));
        log_send(LogType::DEBUG, "json_get_percent_val sent MotorCommandMessage to queue with pos=%u!", cmd.pos);

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

        // Subscribe to topic MQTT_POS_REQUEST_TOPIC with QoS 0
        err = mqtt_subscribe(client, MQTT_POS_REQUEST_TOPIC, 0, mqtt_sub_request_cb, arg);

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

int Wifi::init(Motor *mtr)
{
    mtr_ = mtr;
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