/*
    Button.hh - Handling button push events
    Created by Nick Leslie
*/

#include "Button.hh"

Button::Button(byte pinNum) {
    this->_pin = pinNum;
    pinMode(this->_pin, INPUT);
}

bool Button::isPushed() {
    byte btn_state = digitalRead(this->_pin);
    if (btn_state == HIGH) {
        return true;
    }
    return false;
}
