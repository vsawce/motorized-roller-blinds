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

#include "led.h"
#include "init.h"
#include "motor.h"

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
#define MAIN_TASK_PRIORITY      ( tskIDLE_PRIORITY + 2UL )
#define BLINK_TASK_PRIORITY     ( tskIDLE_PRIORITY + 1UL )
#define WORKER_TASK_PRIORITY    ( tskIDLE_PRIORITY + 4UL )

// Stack sizes of our threads in words (4 bytes)
#define MAIN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define BLINK_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define WORKER_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

#include "pico/async_context_freertos.h"
static async_context_freertos_t async_context_instance;

// Create an async context
static async_context_t *example_async_context(void) {
    async_context_freertos_config_t config = async_context_freertos_default_config();
    config.task_priority = WORKER_TASK_PRIORITY; // defaults to ASYNC_CONTEXT_DEFAULT_FREERTOS_TASK_PRIORITY
    config.task_stack_size = WORKER_TASK_STACK_SIZE; // defaults to ASYNC_CONTEXT_DEFAULT_FREERTOS_TASK_STACK_SIZE
    if (!async_context_freertos_init(&async_context_instance, &config))
        return NULL;
    return &async_context_instance.core;
}

#if USE_LED

void blink_task(__unused void *params) {
    if (params == NULL) {
        vTaskDelete(NULL);  // Delete this task if parameters are invalid
    }

    LED *led_ptr = static_cast<LED *>(params);


    while (true) {
        led_ptr->toggle();
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


int main() {
    LED led;

    //Motor mtr(PIN::ULN2003_IN1, PIN::ULN2003_IN2, PIN::ULN2003_IN3, PIN::ULN2003_IN4, MotorDriveMode::NormalDrive);
    
    //mtr.init();

    stdio_init_all();
    
#if USE_LED
    //Init LED, if fails print
    if (init_wifi_led()) printf("Failed to initialize the CYW43 Wifi/LED\n");
    xTaskCreate(blink_task, "BlinkTask", 256, &led, 1, NULL);
#endif

    // Start the scheduler
    vTaskStartScheduler();


    // while (true) {
    //     printf("Hello, world!!\n");
    //     led.toggle();
    //     //mtr.testContinuousRotationBlocking(MotorDriveDirection::Forward);
    //     mtr.testOneFullRotationBlocking(MotorDriveDirection::Forward);
    //     //sleep_ms(500);
    // }
} 
