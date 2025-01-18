/**
 *  Designed by Vincent Saw
 * 
 */

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/cyw43_arch.h"

#include "FreeRTOS.h"
#include "task.h"

#include "definitions.h"
#include "led.h"
#include "init.h"
#include "motor.h"

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
#define LED_BUSY_WAIT 1
#endif

// Delay between led blinking
#define LED_DELAY_MS 1000

// Priorities of our threads - higher numbers are higher priority
#define USB_DEBUG_TASK_PRIORITY     ( tskIDLE_PRIORITY + 3UL )  //Highest
#define MOTOR_TASK_PRIORITY         ( tskIDLE_PRIORITY + 2UL )
#define BLINK_TASK_PRIORITY         ( tskIDLE_PRIORITY + 1UL )  //Lowest

// Stack sizes of our threads in words (4 bytes)
#define USB_DEBUG_TASK_STACK_SIZE   ( configMINIMAL_STACK_SIZE + 128 )
#define MOTOR_TASK_STACK_SIZE       ( configMINIMAL_STACK_SIZE + 64 )
#define BLINK_TASK_STACK_SIZE       ( configMINIMAL_STACK_SIZE )

// Logging definitions
#define LOG_QUEUE_SIZE 10           // Maximum number of messages in the queue
#define LOG_MESSAGE_MAX_LENGTH 64   // Maximum length of each log message


//##################################//
//          ???????????             //
//##################################//

#include "pico/async_context_freertos.h"
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

QueueHandle_t logQueue;           // Global log queue

// Thread-safe log sending function
void send_log(const char *message)
{
    if (logQueue != NULL) {
        if (xQueueSend(logQueue, message, pdMS_TO_TICKS(100)) != pdPASS) {
            // Handle queue full (optional, TODO?)
            //printf("Log queue full. Message dropped: %s\n", message); // Add this for debugging
        }
    }
}


//##################################//
//              TASKS               //
//##################################//
///////////////////
// BLINK TASK
///////////////////

#if USE_LED
void blink_task(__unused void *params)
{
    if (params == NULL) {
        send_log("Blink task: Invalid parameters");
        vTaskDelete(NULL);  // Delete this task if parameters are invalid
    }

    LED *led_ptr = static_cast<LED *>(params);

    while (true) {
        led_ptr->toggle();
        send_log("Toggled");
        vTaskDelay(pdMS_TO_TICKS(500));
    }

#if LED_BUSY_WAIT
        // You shouldn't usually do this. We're just keeping the thread busy,
        // experiment with BLINK_TASK_PRIORITY and LED_BUSY_WAIT to see what happens
        // if BLINK_TASK_PRIORITY is higher than TEST_TASK_PRIORITY main_task won't get any free time to run
        // unless configNUMBER_OF_CORES > 1
        busy_wait_ms(LED_DELAY_MS);
#else
        sleep_ms(LED_DELAY_MS);
#endif
}
#endif // USE_LED

///////////////////
//  MOTOR TASK
///////////////////

//Can remove typedef if in C++ do get similar typedef struct behavior from C
struct MotorTaskParams
{
    Motor *s_mtr_ptr;         // Pointer to the LED object
    MotorDriveDirection *s_motorDriveDir_ptr; // Pointer to the blink delay value
};

void motor_task(void *pvParameters)
{
    if (pvParameters == NULL) {
        send_log("Motor task: Invalid parameters");
        vTaskDelete(NULL);  // Delete this task if parameters are invalid
    }

    MotorTaskParams *params = (MotorTaskParams *)pvParameters;
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
//  USB DEBUG TASK
///////////////////

void usb_debug_task(__unused void *params)
{
    char logBuffer[LOG_MESSAGE_MAX_LENGTH];

    while (true) {
        // Wait for a log message from the queue
        if (xQueueReceive(logQueue, logBuffer, portMAX_DELAY) == pdPASS) {
            // Send the log message over USB
            printf("USB Debug Log: %s\n", logBuffer);
        }
    }
}


//##################################//
//              MAIN                //
//##################################//
int main()
{
    LED led;

    Motor mtr(PIN::ULN2003_IN1, PIN::ULN2003_IN2, PIN::ULN2003_IN3, PIN::ULN2003_IN4, MotorDriveMode::NormalDrive);
    mtr.init();
    MotorDriveDirection mtrDrvDir = MotorDriveDirection::Forward;

    stdio_init_all();
    printf("System initializing...\n");
    printf("\"BLINDS::WINDOW_HEIGHT_MM\":%dmm\t\"getWindowHeightLimitSteps()\": %dsteps\n", BLINDS::WINDOW_HEIGHT_MM, mtr.getWindowHeightLimitSteps());
    
#if USE_LED
    //Init LED, if it fails then print
    if (init_wifi_led()) printf("Failed to initialize the CYW43 Wifi/LED\n");
    xTaskCreate(blink_task, "BlinkTask", BLINK_TASK_STACK_SIZE, &led, BLINK_TASK_PRIORITY, NULL);
#endif

    logQueue = xQueueCreate(LOG_QUEUE_SIZE, LOG_MESSAGE_MAX_LENGTH);
    if (logQueue == NULL) {
        printf("Failed to create log queue\n");
        return 1;
    }

    xTaskCreate(usb_debug_task, "USBDebugTask", USB_DEBUG_TASK_STACK_SIZE, NULL, USB_DEBUG_TASK_PRIORITY, NULL);

    MotorTaskParams mtParams = {
        .s_mtr_ptr = &mtr,                  // Pass the address of the LED object
        .s_motorDriveDir_ptr = &mtrDrvDir   // Pass the address of the motor drive direction variable
    };

    xTaskCreate(motor_task, "MotorTask", MOTOR_TASK_STACK_SIZE, &mtParams, MOTOR_TASK_PRIORITY, NULL);

    // Start the scheduler
    vTaskStartScheduler();

} 
