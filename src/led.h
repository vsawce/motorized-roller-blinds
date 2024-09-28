#ifndef LED_H //Prevent multiple definitions of same header
#define LED_H

#include "pico/cyw43_arch.h"
#include "definitions.h"

//For built-in LED

class LED
{
    public:
        LED();
        void init();
        void on();
        void off();
        void toggle();

    private:
        uint8_t gpio_led;
};

#endif