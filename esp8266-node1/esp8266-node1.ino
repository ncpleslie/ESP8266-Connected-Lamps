// Boardtype: LOLIN(WEMOS) D1 R2 & Mini

// Adafruit LED Ring
#include <Adafruit_NeoPixel.h>

//FirebaseESP8266.h must be included before ESP8266WiFi.h
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

// Access Point information
// Will be displayed when the user initially starts the device
// or the device is unable to detect a known WIFI access point
#define AP_NAME "Anya's-Lamp"

// LED Strip/Ring data
#define LED_PIN   D1
#define LED_COUNT 24
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
int led_strip_brightness = 50;

// I used an additional LED to display trigger state
// This is optional
// Disable it here if not used
// Trigger state can still be displayed with the LED strip
bool INTERNAL_LED = true;
int INTERNAL_LED_PIN = 16;

// Button Pin
// Used to trigger the lamp and switch the state on Firebase RTDB
int BTN_PIN = 4;
bool reset_btn_state = false;

// FIREBASE RTDB Information
// Add firebase information here
#define FIREBASE_HOST ""
#define FIREBASE_AUTH ""
bool swState = false;
// Name of the field that this device alters on Firebase
String nodeName = "LED_STATUS_Node1";
// Name of the field that this device checks to see if it should turn on the LEDs
String partnerNodeName = "LED_STATUS_Node2";

String nodeID = "node1";
FirebaseData firebaseData;

// Board Information
int BAUDRATE = 9600;

void setup()
{
  Serial.begin(BAUDRATE);
  pinMode(INTERNAL_LED_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT);

  startWifiManager();
  connectToDatabase();
  initDatabaseState();
  initLEDStrip();
}

void loop()
{
  handleButtonEvent();
  handleLampEvent();
  delay(2000);
}

//
// Device code starts here
//
void handleButtonEvent() {
  // For Push btn response LED
  int btn_state = digitalRead(BTN_PIN);

  // If button push
  if (btn_state == HIGH) {
    enableSingleLED();
    setDatabaseRecord(true);
    reset_btn_state = true;
    delay(1000);
  }

  // If button released
  // reset the state of the database for this node
  if (btn_state == LOW && reset_btn_state) {
    reset_btn_state = false;
    disableLED();
    setDatabaseRecord(false);
    delay(1000);
  }
}

// Turn on the ring/strip when the other user pushes their button
void handleLampEvent() {
  // For LED Ring
  getDatabaseRecord();
  if (firebaseData.boolData()) {
    enableFullLED();
  }
  else if (!firebaseData.boolData()) {
    disableLED();
  }
}

void startWifiManager() {
  // Wifi manager will connect to a pre-existing WIFI network
  // If no WIFI network is found the device will become an AP to set/found a new WIFI network
  WiFiManager wifiManager;
  //first parameter is name of access point, second is the password
  wifiManager.autoConnect(AP_NAME);
}

//
// Firebase Database Code starts here
//
void connectToDatabase() {
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void initDatabaseState() {
  // Sets this node's (this lamp) state, on the database, to be false
  // Ensures potential "Stuck states" of the node will be fix on boot
  Firebase.setBool(firebaseData, nodeName, swState);
}

void setDatabaseRecord(bool state) {
  Firebase.setBool(firebaseData, nodeName, state);
}

void getDatabaseRecord() {
  Firebase.getBool(firebaseData, partnerNodeName);
}

//
// LED Strip code starts here
//
void initLEDStrip() {
  strip.begin();
  strip.show();
  strip.setBrightness(led_strip_brightness);
}

void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void enableSingleLED() {
  // When button push then turn on the leds to show user
  if (INTERNAL_LED) {
    digitalWrite(INTERNAL_LED_PIN, HIGH);
  }
  uint32_t red = strip.Color(255, 0, 0);
  strip.setPixelColor(2, red);
  strip.setPixelColor(3, red);
  strip.show();
  delay(50);
}

void disableLED() {
  if (INTERNAL_LED) {
    digitalWrite(INTERNAL_LED_PIN, LOW);
  }
  strip.clear();
  strip.show();
}

void enableFullLED() {
  colorWipe(strip.Color(255,   0,   0), 50); // Red
  colorWipe(strip.Color(  0, 255,   0), 50); // Green
  colorWipe(strip.Color(  0,   0, 255), 50); // Blue
}
