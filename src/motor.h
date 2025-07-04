#ifndef MOTOR_H //Prevent multiple definitions of same header
#define MOTOR_H

#include "pico/stdlib.h"
#include <stdio.h>
#include <limits>

//RTOS task
#include "FreeRTOS.h"
#include "task.h" 
#include "queue.h"
#include "timers.h"

#include "logger.h"
#include "definitions.h"

// Logging definitions
#define CMD_QUEUE_SIZE 5           // Maximum number of messages in the queue
#define CMD_TIMEOUT_MS 100

#define MOTOR_RELEASE_TIMEOUT_MS    1000 //Release motor if no new command after x ms

#define BUTTON_STEPS_PER_UPDATE     10  //Number of steps to increase per button update/action

enum class MotorDriveMode {
    WaveDrive,
    NormalDrive,
    HalfStepDrive
};

enum class MotorDriveDirection {
    Forward,
    Reverse
};

enum class MotorCommand {
    ROTATE_TO_PERCENT,
    CALIBRATE,
    BUTTON_UP,
    BUTTON_DOWN,
    STOP
};

struct MotorCommandMessage {
    MotorCommand mc;
    uint32_t pos;   //i.e. percentage
};

class Motor
{
    public:
        Motor(uint8_t pinIn1, uint8_t pinIn2, uint8_t pinIn3, uint8_t pinIn4, MotorDriveMode dm);
        // uint8_t getDriveMode();  // Currently points to step sequence arrays. Need to get MotorDriveMode enum
        uint8_t getNumPhases();
        uint16_t getNumStepsPerFullRotation();
        uint32_t getCurrentStepPos();
        uint32_t getWindowHeightLimitSteps();
        QueueHandle_t getCommandQueue(); //QueueHandle_t already pointer
        void init();    //Inits GPIO
        uint8_t processCommands(TimerHandle_t mr_th);     //All-inclusive function to drive queued motor commands. Ret 0 if no command processed during call
        void set_step(uint8_t phase);
        void calibrateCurrentStepPosZero();     //Call when motor is in zero'd position
        void rotateNumSteps(MotorDriveDirection dir, uint32_t numSteps, bool disable_limit = false);    //Rotate based on # steps
        void rotateNumFullRotations(MotorDriveDirection dir, uint8_t numRotations);     //Rotate shaft amount of degrees
        void rotateToPercent(uint8_t percent);   //Rotate to a specified percentage
        void rotateLinearHeightMillimeters(MotorDriveDirection dir, uint8_t height_mm); //Rotate linear distance

        static void releaseMotor();                    //Turn motor GPIO off. Static because needs non-member access from timer callback func

    private: //m_ naming convention for private member variables
        const uint8_t   *m_driveMode;
        uint8_t         m_numPhases;
        uint16_t        m_numStepsPerFullRotation;
        int32_t        m_currentStepPos;
        uint32_t        m_windowHeightLimitSteps;
        QueueHandle_t   m_commandQueue;
        
};

#endif