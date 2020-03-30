/*
    LEDStrip.cpp - For the pretty lights using Adafruit LED Ring
    Created by Nick Leslie
*/

#include "LEDStrip.hh"

LEDStrip::LEDStrip(uint16_t ledCount, uint16_t pin, bool internalPresent, uint8_t internalPin) {
  _initLEDStrip(ledCount, pin);
  RED = strip.Color(255, 0, 0);
  GREEN = strip.Color(0, 255, 0);
  BLUE = strip.Color(0, 0, 255);
  YELLOW = strip.Color(255, 255, 0);
  PURPLE = strip.Color(255, 0, 255);
  CYAN = strip.Color(0, 255, 255);
  WHITE = strip.Color(255, 255, 255);
  _internalLED = internalPresent;
  _internalPin = internalPin;
}

void LEDStrip::_initLEDStrip(uint16_t ledCount, uint16_t pin) {
  if (_internalLED) {
    pinMode(_internalPin, OUTPUT);
  }
  strip.updateLength(ledCount);
  strip.setPin(pin);
  strip.updateType(NEO_GRB + NEO_KHZ800);
  strip.begin();
  strip.show();
  strip.setBrightness(LED_BRIGHTNESS);
}

void LEDStrip::enableSingleLED() {
  // When button push then turn on the leds to show user
  if (_internalLED) {
    digitalWrite(_internalPin, HIGH);
  }
  strip.setPixelColor(2, strip.Color(255, 0, 0));
  strip.setPixelColor(3, strip.Color(255, 0, 0));
  strip.show();
  delay(50);
}

void LEDStrip::enableFullLED() {
  // Turn on a random LED sequence
  _heartbeat();
  //  _colorWipe(RED, 50);
  //  _colorWipe(GREEN, 50);
  //  _colorWipe(BLUE, 50);
}

void LEDStrip::disableLED() {
  if (_internalLED) {
    digitalWrite(_internalPin, LOW);
  }
  strip.clear();
  strip.show();
}

void LEDStrip::_colorWipe(uint32_t color, int wait) {
  //  For each pixel in strip...
  //  Set pixel's color (in RAM)
  //  Update strip to match
  //  Pause for a moment
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}

void LEDStrip::_breath(uint32_t color) {
  for (int i = 0; i <= 255; i++) {
    strip.fill(color);
    strip.show();
  }
  for (int i = 255; i >= 0; i--) {
    strip.fill(color);
    strip.show();
  }
}

void LEDStrip::_heartbeat() {
  for (int i = 0; i <= 2; i++) {
    strip.fill(RED);
    strip.show();
    delay(50);
    strip.clear();
    strip.show();
    delay(200);
    strip.fill(RED);
    strip.show();
    delay(50);
    strip.clear();
    strip.show();
    delay(2000);
  }
}

void LEDStrip::_flash() {
  for (int i = 0; i <= 10; i++) {
    strip.fill(BLUE);
    strip.show();
    delay(10);
  }
}

void LEDStrip::error() {
  this->_breath(RED);
}

void LEDStrip::waiting() {
  this->_breath(CYAN);
}
