/*
    Button.hh - Handling button push events
    Created by Nick Leslie
*/

#ifndef Button_hh
#define Button_hh

#include <Arduino.h>

class Button {
    private:
        int _pin;
    public:
        Button(int pin);
        bool isPushed();
};

#endif
