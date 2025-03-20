# Motorized Roller Blinds
A Raspberry Pi Pico W RP2040-based smart blinds project for my apartment.

I made this for fun as well as to minimize nocturnal light bleed from my apartment's outdoor lights while allowing myself to wake up to sunlight in the morning.

This system integrates with Home Assistant and MQTT, so a dedicated Home Assistant instance with the MQTT add-on is needed

## Build / Upload Instructions
As of this markdown's commit 
### Linux
1. Clone the [pico-sdk](https://github.com/raspberrypi/pico-sdk) and [FreeRTOS-Kernel](https://github.com/FreeRTOS/FreeRTOS-Kernel) repos
    * Follow their instructions for setup
2. If not done already, create the following system variables and assign them to pico-sdk and FreeRTOS-Kernel paths
    * Example in `~/.bashrc`:
      ```
      #PI PICO SDK
      export PICO_SDK_PATH=/home/<user>/pico-sdk/
      export FREERTOS_KERNEL_PATH=/home/<user>/FreeRTOS-Kernel/
      ```
    * `source ~/.bashrc` after
3. Clone this repo
4. `cd` into this repo's root directory
5. `mkdir build`
6. `cd build`
7. Run cmake in this directory
    * **Required options/arguments**:
      * `-DPICO_BOARD=pico_w`
      * `-DWIFI_SSID="<your_wifi_ssid>"`
      * `-DWIFI_PASSWORD="<your_wifi_pass>"`
      * `-DMQTT_CLIENT_ID="<custom_client_id>"`
        * Should be a custom and unique ID for each Pico W device. You choose.
      * `-DMQTT_USER=<"home_assistant_user">`
        * Should be the helper user you created specifically Home Assistant for MQTT
      * `-DMQTT_PASSWORD=<"home_assistant_pass">`
        * Should be the helper user's password you created specifically Home Assistant for MQTT
    * Example call: `cmake -DPICO_BOARD=pico_w -DWIFI_SSID="eduroam" -DWIFI_PASSWORD="abc123" -DMQTT_CLIENT_ID="picow_0" -DMQTT_USER="mqtt_test" -DMQTT_PASSWORD="xyz123" ..`
8. **Ensure the following user-defined macros in the program are correct**
    * definitions.h:
      * In `namespace BLINDS`, make sure your window height and blinds' shaft diameter is set accurately
      * In `namespace PIN`, make sure GPIO pins are set properly
    * wifi.h (may move these to definitions.h in the future):
      * `MQTT_ADDR` - point to the IP your Home Assistant is running on
      * `MQTT_ADDR_PORT` - point to the port your Home Assistant is configured to for MQTT
      * `MQTT_POS_REQUEST_TOPIC` - if you want to set a custom MQTT topic for position request (I use `/blinds/<MQTT_CLIENT_ID>/pos_request`)
      * `MQTT_POS_JSON_KEY` - JSON key string for Home Assistant -> Pico W blinds % position command (My example json: `{"pos":"50%"}`)
9. `make main` to build once all variables are set
10. Copy `src/main.uf2` into your Pico W and start a serial terminal!

## Home Assistant Example Config
As of Home Assistant Core 2025.3.3 and Home Assistant Operating System 15.0 
</br>
* After installing MQTT add-on and creating a dedicated Home Assistant user for MQTT:
  * Settings->Devices & services->MQTT
  * Click the three dots next to the Mosquitto integration entry->Reconfigure
  * Ensure port is set correctly, and then add the dedicated MQTT Home Assistant user's credentials
  * Submit
* Use something like the Studio Code Server add-on to edit `/config/configuration.yaml` (my current config):
  * ```
    # Loads default set of integrations. Do not remove.
    default_config:
    
    # Load frontend themes from the themes folder
    frontend:
      themes: !include_dir_merge_named themes
    
    automation: !include automations.yaml
    script: !include scripts.yaml
    scene: !include scenes.yaml
    
    input_number:
      slider1:
        name: Bedroom Blinds
        initial: 0
        min: 0
        max: 100
        step: 5
    mqtt:
      - cover:
          name: Smart Blinds
          unique_id: smart__blinds
          command_topic: "/blinds/picow_0/command"
          position_topic: "/blinds/picow_0/pos_status"
          set_position_topic: "/blinds/picow_0/pos_req"
          payload_open: "100"
          payload_close: "0"
          position_open: 100
          position_closed: 0
    ```
* Set up an automation with similar YAML:
  * ```
    - id: <AUTO_ASSIGNED_BY_HOMEASSISTANT?>
    alias: Adjust Blinds Position
    description: ""
    triggers:
      - trigger: state
        entity_id: input_number.slider1
    conditions: []
    actions:
      - action: mqtt.publish
        data_template:
          qos: "0"
          topic: "/blinds/picow_0/pos_request"
          payload: "{\"pos\":\"{{ states('input_number.slider1') | int }}%\"}"
    mode: single
    ```
    
      

