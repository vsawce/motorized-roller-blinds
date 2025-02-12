#include "button.h"

void Button::init()
{
    gpio_init(PIN::BUTTON_UP);
    gpio_set_dir(PIN::BUTTON_UP, GPIO_IN);

    gpio_init(PIN::BUTTON_DOWN);
    gpio_set_dir(PIN::BUTTON_DOWN, GPIO_IN);

    gpio_init(PIN::BUTTON_CALIB);
    gpio_set_dir(PIN::BUTTON_CALIB, GPIO_IN);
}

uint8_t Button::read(uint8_t gpio)
{
    return gpio_get(gpio);
}

uint8_t Button::readButtonUp()
{
    return read(PIN::BUTTON_UP);
}

uint8_t Button::readButtonDown()
{
    return read(PIN::BUTTON_DOWN);
}

uint8_t Button::readButtonCalib()
{
    return read(PIN::BUTTON_CALIB);
}