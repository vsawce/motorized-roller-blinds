#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "pico/stdlib.h"

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