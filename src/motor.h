#ifndef MOTOR_H //Prevent multiple definitions of same header
#define MOTOR_H

#include "pico/stdlib.h"

class Motor
{
    public:
        Motor(uint8_t pinIn1, uint8_t pinIn2, uint8_t pinIn3, uint8_t pinIn4);
        void init();    //Inits GPIO
        void set_step(uint8_t phase);
        void testContinuousRotationBlocking();    //Continuously spins motor
        // void on();
        // void off();
        // void toggle();

    private: //m_ naming convention for private member variables
        uint8_t     m_pos;
        uint16_t    m_fSteps;  
        
};

#endif