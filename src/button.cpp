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

void    Button::update(Motor *m)
{
    MotorCommandMessage cmd;
    cmd.pos = 0; //Default pos to prevent null ref
    //Active low
    if (!readButtonUp()) {
        cmd.mc = MotorCommand::BUTTON_UP;
        xQueueSend(m->getCommandQueue(), &cmd, pdMS_TO_TICKS(CMD_TIMEOUT_MS));
    }
    if (!readButtonDown()) {
        cmd.mc = MotorCommand::BUTTON_DOWN;
        xQueueSend(m->getCommandQueue(), &cmd, pdMS_TO_TICKS(CMD_TIMEOUT_MS));
    }
    if (!readButtonCalib()) {
        cmd.mc = MotorCommand::CALIBRATE;
        xQueueSend(m->getCommandQueue(), &cmd, pdMS_TO_TICKS(CMD_TIMEOUT_MS));
    }

    // vTaskDelay(pdMS_TO_TICKS(10)); //10ms delay
}