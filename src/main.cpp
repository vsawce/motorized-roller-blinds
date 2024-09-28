/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "led.h"
#include "init.h"

int main() {
    LED led;

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
        sleep_ms(500);
    }
} 
