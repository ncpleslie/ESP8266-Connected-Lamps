/*
    Button.hh - Handling button push events
    Created by Nick Leslie
*/

#include "Button.hh"

Button::Button(int pinNum) {
    this->_pin = pinNum;
    pinMode(this->_pin, INPUT);
}

bool Button::isPushed() {
    int btn_state = digitalRead(this->_pin);
    if (btn_state == HIGH) {
        return true;
    }
    return false;
}
