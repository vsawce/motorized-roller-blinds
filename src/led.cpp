#include "led.h"

LED::LED()
{
    LED::gpio_led = CYW43_PIN::LED;
}

void LED::init()
{
    //gpio_init(gpio_led);
}

void LED::on()
{
    cyw43_arch_gpio_put(gpio_led, 1);
}

void LED::off()
{
    cyw43_arch_gpio_put(gpio_led, 0);
}

void LED::toggle()
{
    cyw43_arch_gpio_put(gpio_led, !cyw43_arch_gpio_get(gpio_led));
}