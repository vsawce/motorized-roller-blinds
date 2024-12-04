#include "motor.h"

//   Phase ABCD Coils
//   0     1000  A
//   1     1100  AB
//   2     0100   B
//   3     0110   BC
//   4     0010    C
//   5     0011    CD
//   6     0001     D
//   7     1001  A  D

constexpr uint8_t phaseMap[] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x09};

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

void Motor::test()
{
    uint8_t pos = 0;

    while(1) {
        gpio_put(pinIn[pos], 1);
        for (uint8_t i = 0; i < 4; i++) {
            if (i != pos) gpio_put(pinIn[i], 0);
        }
        pos++;
        if (pos == 4) pos = 0;
        sleep_ms(10);
    }
}