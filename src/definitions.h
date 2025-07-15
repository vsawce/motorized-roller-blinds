#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "pico/stdlib.h"

//////////////////////////
//  CYW43 PARAMETERS    //
//////////////////////////

namespace CYW43
{
    constexpr int WIFI_TIMEOUT_MS = 30000; //30s timeout
}

//////////////////////////
//  BLIND PARAMETERS    //
/////////////////////////

namespace BLINDS
{
    constexpr uint8_t SHAFT_DIAMETER_MM = 40;               // ~4cm shaft
    constexpr uint16_t WINDOW_HEIGHT_MM = 1778 + 120;       // 70 inch window (1778mm) + ~5 in buffer
}

//////////////
//  PINS    //
//////////////

namespace PIN
{ 
    // constexpr for compile-time evalutation (modern C++?)
    constexpr uint8_t ULN2003_IN1   = 2;    //GPIO2
    constexpr uint8_t ULN2003_IN2   = 3;    //GPIO3
    constexpr uint8_t ULN2003_IN3   = 4;    //GPIO4
    constexpr uint8_t ULN2003_IN4   = 5;    //GPIO5

    constexpr uint8_t BUTTON_UP     = 22;   //GPIO22, move blinds up
    constexpr uint8_t BUTTON_DOWN   = 21;   //GPIO21, move blinds down
    constexpr uint8_t BUTTON_CALIB  = 20;   //GPIO20, calibrate blind position
}

namespace CYW43_PIN
{
    constexpr uint8_t LED = 0;
}



#endif