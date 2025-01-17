#include "motor.h"

constexpr uint16_t PI_TIMES_100 = 314; //Integer to skip FP math

//  NORMAL DRIVE (more power, more torque)
//  -----------
//   Phase DCBA Val
//   0     0011 0x3
//   1     0110 0x6
//   2     1100 0xC
//   3     1001 0x9
constexpr uint8_t normalDriveStepSequence[4] = {0x3, 0x6, 0xC, 0x9};

//  WAVE DRIVE (less power, but less torque)
//  -----------
//   Phase DCBA Val
//   0     0001 0x1
//   1     0010 0x2
//   2     0100 0x4
//   3     1000 0x8
constexpr uint8_t waveDriveStepSequence[4] = {0x1, 0x2, 0x4, 0x8};

//  HALF STEP DRIVE (Slower, but more accuracy)
//  -----------
//   Phase DCBA Val
//   0     0001 0x1
//   1     0011 0x3
//   2     0010 0x2
//   3     0110 0x6
//   4     0100 0x4
//   5     1100 0xC
//   6     1000 0x8
//   7     1001 0x9
constexpr uint8_t halfStepDriveStepSequence[8] = {0x1, 0x3, 0x2, 0x6, 0x4, 0xc, 0x8, 0x9};

constexpr uint8_t NUM_PINS = 4;

//  5.625 deg / 64 steps
//  (360 deg / 1 full rev) * (64 steps / 5.625 deg) = 64 * 64 = 2048 steps per full rev
constexpr uint16_t STEPS_PER_FULL_REV = 2048;

////////////////////////////////


uint8_t pinIn[4];

Motor::Motor(uint8_t pinIn1, uint8_t pinIn2, uint8_t pinIn3, uint8_t pinIn4, MotorDriveMode dm)
{
    // m_fSteps = fSteps;
    pinIn[0] = pinIn1;
    pinIn[1] = pinIn2;
    pinIn[2] = pinIn3;
    pinIn[3] = pinIn4;

    if (dm == MotorDriveMode::NormalDrive) {
        m_driveMode = normalDriveStepSequence;
        m_numPhases = 4;
        m_numStepsPerFullRotation = STEPS_PER_FULL_REV;
    }
    else if (dm == MotorDriveMode::WaveDrive) {
        m_driveMode = waveDriveStepSequence;
        m_numPhases = 4;
        m_numStepsPerFullRotation = STEPS_PER_FULL_REV;
    }
    else { //Half Step Drive
        m_driveMode = halfStepDriveStepSequence;
        m_numPhases = 8; //Half the speed
        m_numStepsPerFullRotation = STEPS_PER_FULL_REV*2;
    }

    m_currentStepPos = 0;

    //Turn linear window height to max # of steps
    m_windowHeightLimitSteps = (m_numStepsPerFullRotation * 100 * BLINDS::WINDOW_HEIGHT_MM) / (BLINDS::SHAFT_DIAMETER_MM * PI_TIMES_100);

}

// uint8_t Motor::getDriveMode()
// {
//     return *m_driveMode;
// }

uint8_t Motor::getNumPhases()
{
    return m_numPhases;
}

uint16_t Motor::getNumStepsPerFullRotation()
{
    return m_numStepsPerFullRotation;
}

uint32_t Motor::getCurrentStepPos()
{
    return m_currentStepPos;
}

uint32_t Motor::getWindowHeightLimitSteps()
{
    return m_windowHeightLimitSteps;
}

void Motor::init()
{
    for (uint8_t i = 0; i < NUM_PINS; i++) //4 GPIO
    {
        gpio_init(pinIn[i]);
        gpio_set_dir(pinIn[i], GPIO_OUT);
    }
}

void Motor::set_step(uint8_t phase)
{
    if (phase < m_numPhases) { //Valid phase number
        for (uint8_t i = 0; i < NUM_PINS; i++) {
            gpio_put(pinIn[i], (m_driveMode[phase] >> i) & 1);
        }
    }
}

void Motor::calibrateCurrentStepPosZero()
{
    m_currentStepPos = 0;
}

//Non-blocking
void Motor::rotateNumSteps(MotorDriveDirection dir, uint32_t numSteps)
{
    if (dir == MotorDriveDirection::Forward) {
        for (uint16_t pos = 0; pos < numSteps; pos++) {
            if (m_currentStepPos == m_windowHeightLimitSteps) { //Release motor?
                printf("Max window height reached! Current/max pos: %u steps\n", m_currentStepPos);
                break;
            }
            m_currentStepPos++;
            set_step(pos % m_numPhases);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    else {
        for (uint16_t pos = numSteps; pos > 0; pos--) {
            if (m_currentStepPos == 0) { //Release motor?
                printf("Min window retraction reached! Current pos is zero!\n");
                break;
            }
            m_currentStepPos--;
            set_step(pos % m_numPhases);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

//Non-blocking
void Motor::rotateNumFullRotations(MotorDriveDirection dir, uint8_t numRotations)
{
    rotateNumSteps(dir, m_numStepsPerFullRotation*numRotations);
}

//Non-blocking (NEED TO VALIDATE DISTANCE ACCURACY)
void Motor::rotateLinearHeightMillimeters(MotorDriveDirection dir, uint8_t height_mm)
{
    uint32_t numStepsToRotate = m_numStepsPerFullRotation * 100; //Times 100 because later divide by PI_TIMES_100 
        
    //If height input would cause to exceed uint32_t limit
    if (height_mm > std::numeric_limits<decltype(numStepsToRotate)>::max()/numStepsToRotate) { 
        //send_log("rotateLinearHeightMillimeters exceeded maximum height (based on drive method)!");
        return; //Exit function
    }

    //Perform rest of calculation if input is OK. Multiply first to reduce error
    numStepsToRotate *= height_mm;
    numStepsToRotate /= BLINDS::SHAFT_DIAMETER_MM * PI_TIMES_100;
    
    //printf("Rotating %d steps\n", numStepsToRotate);
   
    rotateNumSteps(dir, numStepsToRotate);
}