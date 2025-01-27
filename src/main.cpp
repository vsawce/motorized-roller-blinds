/**
 *  Designed by Vincent Saw
 * 
 */
#include <stdio.h>
#include "pico/stdlib.h"
//#include "pico/multicore.h"
#include "pico/cyw43_arch.h"

#include "FreeRTOS.h"
#include "task.h"

#include "definitions.h"
#include "led.h"
#include "init.h"
#include "motor.h"
#include "logger.h"

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
#define LED_DELAY_MS 1000

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

struct BlinkTaskParams
{
    Logger  *s_log_ptr;  // Pass the address of the Log object
    LED     *s_led_ptr;  // Pass the address of the LED object
};

void blink_task(void *pvParameters)
{
    if (pvParameters == NULL) {
        //send_log("Blink task: Invalid parameters");
        vTaskDelete(NULL);  // Delete this task if parameters are invalid
    }

    BlinkTaskParams *params = (BlinkTaskParams *)pvParameters;

    Logger  *log_ptr = params->s_log_ptr;
    log_ptr->send("blink_task started\n");

    LED     *led_ptr = params->s_led_ptr;

    while (true) {
        led_ptr->toggle();
        log_ptr->send("Toggled");
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

//Can remove typedef if in C++ do get similar typedef struct behavior from C
struct MotorTaskParams
{
    Logger  *s_log_ptr;  // Pass the address of the Log object
    Motor *s_mtr_ptr;         // Pointer to the LED object
};

void motor_task(void *pvParameters)
{
    if (pvParameters == NULL) {
        //send_log("Motor task: Invalid parameters");
        vTaskDelete(NULL);  // Delete this task if parameters are invalid
    }

    MotorTaskParams *params = (MotorTaskParams *)pvParameters;

    Logger  *log_ptr = params->s_log_ptr;
    log_ptr->send("motor_task started\n");

    Motor *mtr_ptr = params->s_mtr_ptr;

    while (true) {
        //Test code to rotate the shaft back and forth
        mtr_ptr->rotateToPercent(0);
        vTaskDelay(pdMS_TO_TICKS(1000));

        mtr_ptr->rotateToPercent(50);
        vTaskDelay(pdMS_TO_TICKS(1000));

        mtr_ptr->rotateToPercent(100);
        vTaskDelay(pdMS_TO_TICKS(1000));

        mtr_ptr->rotateToPercent(50);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}

///////////////////
//  LOGGER TASK
///////////////////

void logger_task(void *params)
{
    if (params == NULL) {
        //send_log("Blink task: Invalid parameters");
        vTaskDelete(NULL);  // Delete this task if parameters are invalid
    }

    Logger *log_ptr = static_cast<Logger *>(params);
    log_ptr->send("logger_task started\n");

    while (true) {
        log_ptr->receive();
    }
}

///////////////////
// WIFI TASK
///////////////////
//Can remove typedef if in C++ do get similar typedef struct behavior from C
struct WifiTaskParams
{
    Logger  *s_log_ptr;     // Pass the address of the Log object
    Wifi    *s_wifi_ptr;    // Pointer to the LED object
};

void wifi_task(void *pvParameters)
{
    if (pvParameters == NULL) {
        //send_log("Motor task: Invalid parameters");
        vTaskDelete(NULL);  // Delete this task if parameters are invalid
    }

    LED led;
    Motor mtr(PIN::ULN2003_IN1, PIN::ULN2003_IN2, PIN::ULN2003_IN3, PIN::ULN2003_IN4, MotorDriveMode::NormalDrive);
    mtr.init();

    //printf("\"BLINDS::WINDOW_HEIGHT_MM\":%dmm\t\"getWindowHeightLimitSteps()\": %dsteps\n", BLINDS::WINDOW_HEIGHT_MM, mtr.getWindowHeightLimitSteps());

    WifiTaskParams *params = (WifiTaskParams *)pvParameters;

    Logger  *log_ptr    = params->s_log_ptr;
    log_ptr->send("wifi_task started\n");

    Wifi    *wifi_ptr   = params->s_wifi_ptr;

    log_ptr->send("Initializing wifi...\n");
    if (cyw43_arch_init()) {
        log_ptr->send("Failed to init CYW43 Wifi & LED\n");
    }

    log_ptr->send("Enabling WiFi station mode...\n");
    wifi_ptr->enableStationMode();
    
    log_ptr->send("Connecting to wifi SSID %s ...\n", WIFI_SSID);
    if (wifi_ptr->connectToWifi(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, CYW43::WIFI_TIMEOUT_MS)) {
        log_ptr->send("Failed to connect to wifi SSID %s . Timeout: \n", WIFI_SSID);
    }
    else {
        log_ptr->send("Connected to wifi SSID %s !\n", WIFI_SSID);
    }

#if USE_LED
    //Init LED, if it fails then print
    BlinkTaskParams btParams = {
        .s_log_ptr = log_ptr,                  // Pass the address of the Log object
        .s_led_ptr = &led,                  // Pass the address of the LED object
    };
    //if (init_wifi_led()) printf("Failed to initialize the CYW43 Wifi/LED\n");
    xTaskCreate(blink_task, "BlinkTask", BLINK_TASK_STACK_SIZE, &btParams, BLINK_TASK_PRIORITY, NULL);
#endif

    MotorTaskParams mtParams = {
        .s_log_ptr = log_ptr,                  // Pass the address of the Log object
        .s_mtr_ptr = &mtr,                  // Pass the address of the LED object
    };

    xTaskCreate(motor_task, "MotorTask", MOTOR_TASK_STACK_SIZE, &mtParams, MOTOR_TASK_PRIORITY, NULL);

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
    Logger log;

    Wifi wifi;

    if (init_logger(&log)) printf("Failed to initialize logger\n");
    xTaskCreate(logger_task, "LoggerTask", LOGGER_TASK_STACK_SIZE, &log, LOGGER_TASK_PRIORITY, NULL);
    
    WifiTaskParams wifiParams = {
        .s_log_ptr  = &log,                  // Pass the address of the Log object
        .s_wifi_ptr = &wifi,                  // Pass the address of the LED object
    };
    // Create the Wi-Fi task on Core 0
    TaskHandle_t wifi_task_handle;
    xTaskCreate(wifi_task, "WifiTask", WIFI_TASK_STACK_SIZE, &wifiParams, WIFI_TASK_PRIORITY, &wifi_task_handle);

    // Set the task to run on Core 1
    // vTaskCoreAffinitySet(wifi_task_handle, 1);  // Bind the Wi-Fi task to Core 1

    // Start the scheduler
    vTaskStartScheduler();

} 
