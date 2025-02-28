/**
 *  Designed by Vincent Saw
 * 
 */
#include <stdio.h>
#include "pico/stdlib.h"
//#include "pico/multicore.h"
#include "pico/cyw43_arch.h"
#include "hardware/watchdog.h"

#include "lwip/ip4_addr.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

// Inclusion of C program
#ifdef __cplusplus
extern "C" {
#endif

#include "ping.h"

#ifdef __cplusplus
}
#endif

#include "definitions.h"
#include "led.h"
#include "wifi.h"
#include "motor.h"
#include "logger.h"
#include "button.h"

//##################################//
//          USER MACROS             //
//##################################//

// Which core to run on if configNUMBER_OF_CORES==1
#ifndef RUN_FREE_RTOS_ON_CORE
#define RUN_FREE_RTOS_ON_CORE 0
#endif

// Whether to flash the led
#ifndef USE_LED
#define USE_LED 1
#endif

// Whether to busy wait in the led thread
#ifndef LED_BUSY_WAIT
#define LED_BUSY_WAIT 0
#endif

// Delay between led blinking
#define LED_DELAY_MS    1000
// Watchdog timer duration
#define WDT_DURATION_MS 5000

// Ping google
#ifndef PING_ADDR
#define PING_ADDR "8.8.8.8"
#endif

// Priorities of our threads - higher numbers are higher priority
#define LOGGER_TASK_PRIORITY        ( tskIDLE_PRIORITY + 4UL )  //Highest
#define WIFI_TASK_PRIORITY         ( tskIDLE_PRIORITY + 3UL )
#define MOTOR_TASK_PRIORITY         ( tskIDLE_PRIORITY + 2UL )
#define BLINK_TASK_PRIORITY         ( tskIDLE_PRIORITY + 1UL )  //Lowest

// Stack sizes of our threads in words (4 bytes)
#define LOGGER_TASK_STACK_SIZE      ( configMINIMAL_STACK_SIZE + 128 )
#define WIFI_TASK_STACK_SIZE        ( configMINIMAL_STACK_SIZE + 512 )
#define MOTOR_TASK_STACK_SIZE       ( configMINIMAL_STACK_SIZE + 64 )
#define BLINK_TASK_STACK_SIZE       ( configMINIMAL_STACK_SIZE )

//##################################//
//          ???????????             //
//##################################//

// #include "pico/async_context_freertos.h"
// static async_context_freertos_t async_context_instance;

// // Create an async context
// static async_context_t *example_async_context(void) {
//     async_context_freertos_config_t config = async_context_freertos_default_config();
//     config.task_priority = WORKER_TASK_PRIORITY; // defaults to ASYNC_CONTEXT_DEFAULT_FREERTOS_TASK_PRIORITY
//     config.task_stack_size = WORKER_TASK_STACK_SIZE; // defaults to ASYNC_CONTEXT_DEFAULT_FREERTOS_TASK_STACK_SIZE
//     if (!async_context_freertos_init(&async_context_instance, &config))
//         return NULL;
//     return &async_context_instance.core;
// }

// Thread-safe log sending function
// void send_log(const char *message)
// {
//     if (logQueue != NULL) {
//         if (xQueueSend(logQueue, message, pdMS_TO_TICKS(100)) != pdPASS) {
//             // Handle queue full (optional, TODO?)
//             //printf("Log queue full. Message dropped: %s\n", message); // Add this for debugging
//         }
//     }
// }


//##################################//
//              TASKS               //
//##################################//

///////////////////
// BLINK TASK
///////////////////

//Can remove typedef if in C++ do get similar typedef struct behavior from C

#if USE_LED
void blink_task(void *pvParameters)
{
    if (pvParameters == NULL) {
        //log_send(LogType::ERROR, "Blink task: Invalid parameters");
        vTaskDelete(NULL);  // Delete this task if parameters are invalid
    }

    log_send(LogType::STANDARD, "blink_task started on core %d\n", portGET_CORE_ID());

    LED     *led_ptr = (LED*)pvParameters;

    while (true) {
        led_ptr->toggle();
        log_send(LogType::DEBUG, "Toggled");
        vTaskDelay(pdMS_TO_TICKS(500));
    }

// #if LED_BUSY_WAIT
//         // You shouldn't usually do this. We're just keeping the thread busy,
//         // experiment with BLINK_TASK_PRIORITY and LED_BUSY_WAIT to see what happens
//         // if BLINK_TASK_PRIORITY is higher than TEST_TASK_PRIORITY main_task won't get any free time to run
//         // unless configNUMBER_OF_CORES > 1
//         busy_wait_ms(LED_DELAY_MS);
// #else
//         sleep_ms(LED_DELAY_MS);
// #endif
}
#endif // USE_LED

///////////////////
//  MOTOR TASK
///////////////////

//  Timer callback function must match the prototype:
//      void vCallbackFunctionExample( TimerHandle_t xTimer );
void vTimerCallback_ReleaseMotor(TimerHandle_t xTimer)
{
    Motor::releaseMotor();
}

void motor_task(void *pvParameters)
{
    if (pvParameters == NULL) {
        //log_send(LogType::ERROR, "Motor task: Invalid parameters");
        vTaskDelete(NULL);  // Delete this task if parameters are invalid
    }

    log_send(LogType::STANDARD, "motor_task started on core %d\n", portGET_CORE_ID());

    Motor *mtr_ptr = (Motor*)pvParameters;

    Button b;

    b.init();

    // Create RTOS timer object to release motor
    TimerHandle_t motorReleaseTimerHandle;
    motorReleaseTimerHandle = xTimerCreate("MotorReleaseTimer", pdMS_TO_TICKS(MOTOR_RELEASE_TIMEOUT_MS), pdFALSE, (void*) 0, vTimerCallback_ReleaseMotor);

    while (true) {
        // Test code to rotate the shaft back and forth
        // mtr_ptr->rotateToPercent(0);
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // mtr_ptr->rotateToPercent(50);
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // mtr_ptr->rotateToPercent(100);
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // mtr_ptr->rotateToPercent(50);
        // vTaskDelay(pdMS_TO_TICKS(1000));
        b.update(mtr_ptr);
        if (mtr_ptr->processCommands(motorReleaseTimerHandle)) {
            xTimerStart(motorReleaseTimerHandle, 0);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }

}

///////////////////
//  LOGGER TASK
///////////////////

void logger_task(__unused void *pvParameters)
{
    log_send(LogType::STANDARD, "logger_task started on core %d\n", portGET_CORE_ID());

    while (true) {
        log_receive(); //Consume log in queue and print it
    }
}

///////////////////
// WIFI TASK
///////////////////

void wifi_task(void *pvParameters)
{
    if (pvParameters == NULL) {
        // printf("Motor task: Invalid parameters");
        vTaskDelete(NULL);  // Delete this task if parameters are invalid
    }

    LED led;
    Motor mtr(PIN::ULN2003_IN1, PIN::ULN2003_IN2, PIN::ULN2003_IN3, PIN::ULN2003_IN4, MotorDriveMode::NormalDrive);
    mtr.init();

    if (watchdog_caused_reboot()) {
        printf("Rebooted by Watchdog?\n");
    } else {
        printf("Clean boot\n");
    }

    watchdog_enable(WDT_DURATION_MS, 1); //If WDT not updated for WDT_DURATION_MS ms, reset. 2nd arg = 1 = pause during debug

    //printf("\"BLINDS::WINDOW_HEIGHT_MM\":%dmm\t\"getWindowHeightLimitSteps()\": %dsteps\n", BLINDS::WINDOW_HEIGHT_MM, mtr.getWindowHeightLimitSteps());

    log_send(LogType::STANDARD, "wifi_task started on core %d\n", portGET_CORE_ID());

    Wifi    *wifi_ptr   = (Wifi*)pvParameters;

    log_send(LogType::STANDARD, "Initializing wifi...\n");
    if (cyw43_arch_init()) {
        log_send(LogType::ERROR, "Failed to init CYW43 Wifi & LED\n");
    }

    watchdog_update(); //Update WDT

    log_send(LogType::STANDARD, "Enabling WiFi station mode...\n");
    wifi_ptr->enableStationMode();

    watchdog_update(); //Update WDT
    
    log_send(LogType::STANDARD, "Connecting to wifi SSID %s ...\n", WIFI_SSID);
    if (wifi_ptr->connectToWifi(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, CYW43::WIFI_TIMEOUT_MS)) {
        log_send(LogType::ERROR, "Failed to connect to wifi SSID %s . Timeout: \n", WIFI_SSID);
    }
    else {
        log_send(LogType::STANDARD, "Connected to wifi SSID %s !\n", WIFI_SSID);
    }

    log_send(LogType::STANDARD, "Assigned IP is: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_list)));

    watchdog_update(); //Update WDT

    watchdog_disable(); //Stop WDT as problematic section is in wifi

    vTaskDelay(1000);

    TaskHandle_t logger_task_handle;
    xTaskCreate(logger_task, "LoggerTask", LOGGER_TASK_STACK_SIZE, NULL, LOGGER_TASK_PRIORITY, &logger_task_handle);
    vTaskCoreAffinitySet(logger_task_handle, 0x1); //Set logger_task to 1st core

#if USE_LED
    //Init LED
    TaskHandle_t blink_task_handle;
    xTaskCreate(blink_task, "BlinkTask", BLINK_TASK_STACK_SIZE, &led, BLINK_TASK_PRIORITY, &blink_task_handle);
    vTaskCoreAffinitySet(blink_task_handle, 0x2); //Set blink_task to 2nd core
#endif

    TaskHandle_t motor_task_handle;
    xTaskCreate(motor_task, "MotorTask", MOTOR_TASK_STACK_SIZE, &mtr, MOTOR_TASK_PRIORITY, &motor_task_handle);
    vTaskCoreAffinitySet(motor_task_handle, 0x2); //Set motor_task to 2nd core

    
    log_send(LogType::STANDARD, "Pinging %s\n", PING_ADDR);
    ip_addr_t ping_addr;
    ipaddr_aton(PING_ADDR, &ping_addr);
    ping_init(&ping_addr);
    
    
    while(true) {
        // not much to do as LED is in another task, and we're using RAW (callback) lwIP API
        vTaskDelay(100);
    }

    cyw43_arch_deinit();
}


//##################################//
//              MAIN                //
//##################################//
int main()
{
    Wifi wifi;

    if (log_init()) printf("Failed to initialize logger\n");

    // Create the Wi-Fi task on Core 0
    TaskHandle_t wifi_task_handle;
    xTaskCreate(wifi_task, "WifiTask", WIFI_TASK_STACK_SIZE, &wifi, WIFI_TASK_PRIORITY, &wifi_task_handle);

    // Set the task to run on Core 1
    // vTaskCoreAffinitySet(wifi_task_handle, 0x1);  // Bind the Wi-Fi task to 2nd core

    // Start the scheduler
    vTaskStartScheduler();

} 
