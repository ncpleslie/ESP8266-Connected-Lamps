/*
    LEDStrip.cpp - For the pretty lights using Adafruit LED Ring
    Created by Nick Leslie
*/

#ifndef LEDStrip_hh
#define LEDStrip_hh

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

class LEDStrip {
  private:
    Adafruit_NeoPixel strip;
    int LED_BRIGHTNESS = 50;
    uint32_t RED;
    uint32_t GREEN;
    uint32_t BLUE;
    uint32_t YELLOW;
    uint32_t PURPLE;
    uint32_t CYAN;
    uint32_t WHITE;

    // I used an additional LED to display trigger state
    // This is optional
    // Disable it here if not used
    // Trigger state can still be displayed with the LED strip
    bool _internalLED;
    uint8_t _internalPin;

    void _initLEDStrip(uint16_t ledCount, uint16_t pin);
    void _colorWipe(uint32_t color, int wait);
    void _heartbeat();
    void _breath(uint32_t color);
    void _flash();

  public:
    LEDStrip(uint16_t ledCount, uint16_t pin, bool internalPresent, uint8_t internalPin);
    void enableSingleLED();
    void enableFullLED();
    void disableLED();
    void error();
    void waiting();
};

#endif
