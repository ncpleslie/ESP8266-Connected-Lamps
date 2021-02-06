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
  ORANGE = strip.Color(255, 165, 0);
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
  strip.setPixelColor(2, GREEN);
  strip.setPixelColor(3, GREEN);
  strip.show();
  delay(50);
}

void LEDStrip::enableFullLED() {
  // Turn on a random LED sequence
  byte numOfLightEffects = 12;
  byte randNum = random(0, numOfLightEffects + 1);
  Serial.println(randNum);

  switch (randNum) {
    case 0:
      _heartbeat();
      break;
    case 1:
      _colorWipe(RED, 50);
      break;
    case 2:
      _colorWipe(RED, 50);
      _colorWipe(GREEN, 50);
      _colorWipe(BLUE, 50);
      break;
    case 3:
      _breath();
      break;
    case 4:
      _heartbeat();
      break;
    case 5:
      _flash();
      break;
    case 6:
      _rainbow(10);
      break;
    case 7:
      _theaterChaseRainbow(20);
      break;
    case 8:
      _theaterChase(PURPLE, 20);
      break;
    case 9:
      _theaterChase(WHITE, 20);
      break;
    case 10:
      _theaterChase(CYAN, 20);
      break;
    case 11:
      _theaterChase(WHITE, 20);
      _theaterChase(CYAN, 20);
      _theaterChase(BLUE, 20);
      _theaterChase(PURPLE, 20);
      _theaterChase(RED, 20);
      _theaterChase(ORANGE, 20);
      _theaterChase(YELLOW, 20);
      _theaterChase(GREEN, 20);
      break;
    case 12:
      _colorWipe(ORANGE, 50);
      break;
    default:
      _colorWipe(ORANGE, 50);
      break;
  }
}

void LEDStrip::disableLED() {
  if (_internalLED) {
    digitalWrite(_internalPin, LOW);
  }
  strip.clear();
  strip.show();
}

void LEDStrip::error() {
  for (byte i = 0; i <= 255; i++) {
    strip.fill(strip.Color(i, 0, 0));
    strip.show();
  }
  for (byte i = 255; i >= 0; i--) {
    strip.fill(strip.Color(i, 0, 0));
    strip.show();
  }
}

void LEDStrip::waiting() {
  for (byte i = 0; i <= 255; i++) {
    strip.fill(strip.Color(0, i, i));
    strip.show();
  }
  for (byte i = 255; i >= 0; i--) {
    strip.fill(strip.Color(0, i, i));
    strip.show();
  }
}

void LEDStrip::success() {
  this->_colorWipe(GREEN, 50);
}

void LEDStrip::_colorWipe(uint32_t color, int wait) {
  //  For each pixel in strip...
  //  Set pixel's color (in RAM)
  //  Update strip to match
  //  Pause for a moment
  for (byte i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}

void LEDStrip::_breath() {
  for (byte i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, i * 10));
    strip.show();
    delay(50);
  }
  for (byte i = strip.numPixels() - 1; i > 0; i--) {
    strip.setPixelColor(i, strip.Color(0, 0, i * 10));
    strip.show();
    delay(50);
  }
}

void LEDStrip::_heartbeat() {
  for (byte i = 0; i <= 2; i++) {
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
  }
}

void LEDStrip::_flash() {
  for (byte i = 0; i <= 10; i++) {
    strip.fill(BLUE);
    strip.show();
    delay(100);
  }
}


void LEDStrip::_rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256)
  {
    for (int i = 0; i < strip.numPixels(); i++)
    { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

void LEDStrip::_theaterChase(uint32_t color, int wait)
{
  for (int a = 0; a < 10; a++)
  { // Repeat 10 times...
    for (int b = 0; b < 3; b++)
    { //  'b' counts from 0 to 2...
      strip.clear(); //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for (int c = b; c < strip.numPixels(); c += 3)
      {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

void LEDStrip::_theaterChaseRainbow(int wait)
{
  int firstPixelHue = 0; // First pixel starts at red (hue 0)
  for (int a = 0; a < 30; a++)
  { // Repeat 30 times...
    for (int b = 0; b < 3; b++)
    { //  'b' counts from 0 to 2...
      strip.clear(); //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for (int c = b; c < strip.numPixels(); c += 3)
      {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int hue = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color);                       // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}
