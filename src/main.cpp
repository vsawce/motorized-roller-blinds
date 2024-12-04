/**
 *  Designed by Vincent Saw
 * 
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "led.h"
#include "init.h"
#include "motor.h"

int main() {
    LED led;
    Motor mtr(PIN::ULN2003_IN1, PIN::ULN2003_IN2, PIN::ULN2003_IN3, PIN::ULN2003_IN4, MotorDriveMode::NormalDrive);

    mtr.init();
    stdio_init_all();
    //led.init_led();
    if (init_wifi_led())
    {
        printf("Failed to initialize the CYW43 Wifi/LED\n");
        while(1); //Hang indefinitely
    }


    while (true) {
        printf("Hello, world!!\n");
        led.toggle();
        //mtr.testContinuousRotationBlocking();
        mtr.testOneFullRotationBlocking();
        //sleep_ms(500);
    }
} 
