#ifndef BUTTON_H //Prevent multiple definitions of same header
#define BUTTON_H

#include "pico/stdlib.h"

#include "definitions.h"

class Button
{
    public:
        void init();    //Inits GPIO
        uint8_t read(uint8_t gpio); //Active low
        uint8_t readButtonUp();     //Active low
        uint8_t readButtonDown();   //Active low
        uint8_t readButtonCalib();  //Active low


    // private: //m_ naming convention for private member variables
        
        
};

#endif /* BUTTON_H */