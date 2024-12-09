#ifndef MOTOR_H //Prevent multiple definitions of same header
#define MOTOR_H

#include "pico/stdlib.h"
#include <limits>

enum class MotorDriveMode {
    WaveDrive,
    NormalDrive,
    HalfStepDrive
};

enum class MotorDriveDirection {
    Forward,
    Reverse
};

class Motor
{
    public:
        Motor(uint8_t pinIn1, uint8_t pinIn2, uint8_t pinIn3, uint8_t pinIn4, MotorDriveMode dm);
        void init();    //Inits GPIO
        void set_step(uint8_t phase);
        void testContinuousRotationBlocking(MotorDriveDirection dir);  //Continuously spins motor
        void testOneFullRotationBlocking(MotorDriveDirection dir);     //Continuously rotates shaft fully once in intervals   
        // void on();
        // void off();
        // void toggle();

    private: //m_ naming convention for private member variables
        const uint8_t   *m_driveMode;
        uint8_t         m_numPhases;
        uint16_t        m_numStepsPerFullRotation;
        
};

#endif