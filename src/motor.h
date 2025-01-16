#ifndef MOTOR_H //Prevent multiple definitions of same header
#define MOTOR_H

#include "pico/stdlib.h"
#include <limits>

//RTOS task
#include "FreeRTOS.h"
#include "task.h" 

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
        // uint8_t getDriveMode();  // Currently points to step sequence arrays. Need to get MotorDriveMode enum
        uint8_t getNumPhases();
        uint16_t getNumStepsPerFullRotation();
        void init();    //Inits GPIO
        void set_step(uint8_t phase);
        void testContinuousRotationBlocking(MotorDriveDirection dir);  //Continuously spins motor
        void testOneFullRotationBlocking(MotorDriveDirection dir);     //Continuously rotates shaft fully once in intervals   
        void rotateNumFullRotations(MotorDriveDirection dir, uint8_t numRotations);   //Rotate shaft amount of degrees


    private: //m_ naming convention for private member variables
        const uint8_t   *m_driveMode;
        uint8_t         m_numPhases;
        uint16_t        m_numStepsPerFullRotation;
        
};

#endif