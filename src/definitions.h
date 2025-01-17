#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "pico/stdlib.h"

//////////////////////////
//  BLIND PARAMETERS    //
/////////////////////////

namespace BLINDS
{
    constexpr uint8_t SHAFT_DIAMETER_MM = 25;       //example value 2.5cm
    constexpr uint16_t WINDOW_HEIGHT_MM  = 1000;    //example value 1m
}

//////////////
//  PINS    //
//////////////

namespace PIN
{ 
    // constexpr for compile-time evalutation (modern C++?)
    constexpr uint8_t ULN2003_IN1 = 2;  //GPIO2
    constexpr uint8_t ULN2003_IN2 = 3;  //GPIO3
    constexpr uint8_t ULN2003_IN3 = 4;  //GPIO4
    constexpr uint8_t ULN2003_IN4 = 5;  //GPIO5
}

namespace CYW43_PIN
{
    constexpr uint8_t LED = 0;
}



#endif