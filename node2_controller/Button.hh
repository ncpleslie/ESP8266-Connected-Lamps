/*
    Button.hh - Handling button push events
    Created by Nick Leslie
*/

#ifndef Button_hh
#define Button_hh

#include <Arduino.h>

class Button {
    private:
        byte _pin;
    public:
        Button(byte pin);
        bool isPushed();
};

#endif
