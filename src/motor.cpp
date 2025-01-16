#include "motor.h"

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

void Motor::testContinuousRotationBlocking(MotorDriveDirection dir)
{
    uint8_t pos;

    if (dir == MotorDriveDirection::Forward) {
        pos = 0;
        while(1) {
            set_step(pos);
            pos++;
            if (pos == m_numPhases) pos = 0;
            sleep_ms(10); //Blocking
        }
    }
    else {
        pos = m_numPhases-1;
        while(1) {
            set_step(pos);
            pos--;
            if (pos == std::numeric_limits<uint8_t>::max()) pos = m_numPhases-1;
            sleep_ms(10); //Blocking
        }
    }
}

void Motor::testOneFullRotationBlocking(MotorDriveDirection dir)
{
    if (dir == MotorDriveDirection::Forward) {
        while(1) {
            for (uint16_t pos = 0; pos < m_numStepsPerFullRotation; pos++) {
                set_step(pos % m_numPhases);
                sleep_ms(10); //Blocking
            }
            sleep_ms(2000); //Wait for two secs
        }
    }
    else {
        while(1) {
            for (uint16_t pos = m_numStepsPerFullRotation; pos > 0; pos--) {
                set_step(pos % m_numPhases);
                sleep_ms(10); //Blocking
            }
            sleep_ms(2000); //Wait for two secs
        }
    }
}

//Non-blocking
void Motor::rotateNumFullRotations(MotorDriveDirection dir, uint8_t numRotations)
{
    if (dir == MotorDriveDirection::Forward) {
        while(1) {
            for (uint16_t pos = 0; pos < m_numStepsPerFullRotation*numRotations; pos++) {
                set_step(pos % m_numPhases);
                vTaskDelay(pdMS_TO_TICKS(10));
            }
            vTaskDelay(pdMS_TO_TICKS(2000)); //Wait for two secs
        }
    }
    else {
        while(1) {
            for (uint16_t pos = m_numStepsPerFullRotation*numRotations; pos > 0; pos--) {
                set_step(pos % m_numPhases);
                vTaskDelay(pdMS_TO_TICKS(10));
            }
            vTaskDelay(pdMS_TO_TICKS(2000)); //Wait for two secs
        }
    }
}