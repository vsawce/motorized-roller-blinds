#include "motor.h"


//  WAVE DRIVE (less power, but less torque)
//  -----------
//   Phase DCBA Val
//   0     0001 0x1
//   1     0010 0x2
//   2     0100 0x4
//   3     1000 0x8
constexpr uint8_t stepSequence[4] = {0x1, 0x2, 0x4, 0x8}; //Wave drive mode 

uint8_t pinIn[4];

Motor::Motor(uint8_t pinIn1, uint8_t pinIn2, uint8_t pinIn3, uint8_t pinIn4)
{
    // m_fSteps = fSteps;
    pinIn[0] = pinIn1;
    pinIn[1] = pinIn2;
    pinIn[2] = pinIn3;
    pinIn[3] = pinIn4;

    m_pos = 0;
}

void Motor::init()
{
    for (uint8_t i = 0; i < 4; i++)
    {
        gpio_init(pinIn[i]);
        gpio_set_dir(pinIn[i], GPIO_OUT);
    }
}

void Motor::set_step(uint8_t phase)
{
    if (phase < 4) { //Valid phase number
        for (uint8_t i = 0; i < 4; i++) {
            gpio_put(pinIn[i], (stepSequence[phase] >> i) & 1);
        }
    }
}

void Motor::testContinuousRotationBlocking()
{
    uint8_t pos = 0;

    while(1) {
        set_step(pos);
        pos++;
        if (pos == 4) pos = 0;
        sleep_ms(10); //Blocking
    }
}