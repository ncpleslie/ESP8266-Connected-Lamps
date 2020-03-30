// Boardtype: LOLIN(WEMOS) D1 R2 & Mini

#include "LEDStrip.hh";
#include "Button.hh";
#include "Database.hh";

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

// Board Information
int BAUDRATE = 9600;
bool reset_btn_state = false;

// CHANGE INFORMATION BELOW

// Access Point information
// Will be displayed when the user initially starts the device
// or the device is unable to detect a known WIFI access point
#define AP_NAME "Anya's-Lamp"

// FIREBASE RTDB Information
// Add firebase information here
String FIREBASE_HOST = "";
String FIREBASE_AUTH = "";

// Name of the field that this device alters on Firebase
String NODE_NAME = "Node1";
// Name of the field that this device checks to see if it should turn on the LEDs
String PARTNER_NODE_NAME = "Node2";
Database db(FIREBASE_AUTH, FIREBASE_HOST, NODE_NAME, PARTNER_NODE_NAME);

// LED Pin and total LED count
uint16_t LED_PIN = D1;
uint16_t LED_COUNT = 24;
// Optional LED that displays the trigger/button state
bool INTERNAL_LED = true;
uint8_t INTERNAL_LED_PIN = 16;
LEDStrip lamp(LED_COUNT, LED_PIN, INTERNAL_LED, INTERNAL_LED_PIN);

// Button Pin
// Used to trigger the lamp and switch the state on the DB
int BTN_PIN = 4;
Button btn(BTN_PIN);

// Recheck DB interval
int INTERVAL = 1000;
unsigned long time_now = 0;

void setup()
{
  Serial.begin(BAUDRATE);
  startWifiManager();
}

void loop()
{
  checkWifiStatus();
  handleButtonEvent();
  Serial.println("Loop");
  if (millis() > time_now + INTERVAL) {
    Serial.println("1");
    time_now = millis();
    handleLampEvent();
  }
}

//
// Device code starts here
//
void handleButtonEvent() {
  // If button push
  if (btn.isPushed() && db.setCurrentNodeRecord(true)) {
    reset_btn_state = true;
    lamp.enableSingleLED();
    delay(1000);
  }

  // If button released
  // reset the state of the database for this node
  if (!btn.isPushed() && reset_btn_state) {
    reset_btn_state = false;
    lamp.disableLED();
    db.setCurrentNodeRecord(false);
  }
}

// Turn on the ring/strip when the other user pushes their button
void handleLampEvent() {
  // For LED Ring
  if (db.getPartnerNodeRecord()) {
    lamp.enableFullLED();
  }
  lamp.disableLED();
}

void checkWifiStatus() {
  while (WiFi.status() != WL_CONNECTED) {
    lamp.error();
    delay(5000);
  }
}

void startWifiManager() {
  // Wifi manager will connect to a pre-existing WIFI network
  // If no WIFI network is found the device will become an AP to set/find a new WIFI network
  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.autoConnect(AP_NAME);
}

void configModeCallback (WiFiManager *myWiFiManager) {
  lamp.waiting();
  delay(5000);
}
