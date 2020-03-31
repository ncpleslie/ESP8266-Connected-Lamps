/*
   Version: 0.5
   Boardtype: LOLIN(WEMOS) D1 R2 & Mini

   ESP8266 Long Distance Lamp - For connecting couples with a reminder that their partner is thinking about them
   Touch your lamp and activate your partner's lamp with random light up reminders

   Using:
    - Firebase Realtime Database (https://firebase.google.com/)
    - Firebase-ESP8266 (https://github.com/mobizt/Firebase-ESP8266)
    - WifiManager (https://github.com/tzapu/WiFiManager)
    - Adafruit NeoPixel (https://github.com/adafruit/Adafruit_NeoPixel)

    Created by: Nick Leslie
    Last Updated: 31/03/2020
*/

#include "LEDStrip.hh";
#include "Button.hh";
#include "Database.hh";

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

// ----------------------- CHANGE INFORMATION BELOW -------------------------

// Access Point information
// Will be displayed when the user initially starts the device
// or if the device is unable to connect to a known WIFI network
#define AP_NAME "Anya's-Lamp"

// FIREBASE RTDB Information
// Add firebase information here
String FIREBASE_HOST = "";
String FIREBASE_AUTH = "";

// Name of the field that this device alters on Firebase
String NODE_NAME = "Node1";
// Name of the field that this device checks to see if it should turn on the LEDs
String PARTNER_NODE_NAME = "Node2";

// ----------------------- CHANGE INFORMATION IF BOARD LAYOUT IS DIFFERENT ---

// LED Pin and total LED count
uint16_t LED_PIN = D1;
uint16_t LED_COUNT = 24;
// Optional LED that displays the trigger/button state
bool INTERNAL_LED = true;
uint8_t INTERNAL_LED_PIN = 16; // Set to random number if not used
// Button Pin
// Used to trigger the lamp and switch the state on the DB
byte BTN_PIN = 4;

// ----------------------- STOP CHANGING INFORMATION -------------------------

Database db(FIREBASE_AUTH, FIREBASE_HOST, NODE_NAME, PARTNER_NODE_NAME);
LEDStrip lamp(LED_COUNT, LED_PIN, INTERNAL_LED, INTERNAL_LED_PIN);
Button btn(BTN_PIN);

// Recheck DB interval
int INTERVAL = 1000;
unsigned long time_now = 0;
int UPDATE_INTERVAL = 60000;
// Board Information
int BAUDRATE = 9600;
bool reset_btn_state = false;
bool wifi_connected = false;

void setup()
{
  Serial.begin(BAUDRATE);
  startWifiManager();
  checkWifiStatus();
  db.resetNodeState();
}

void loop()
{
  checkWifiStatus();
  handleButtonEvent();
  if (millis() > time_now + INTERVAL) {
    time_now = millis();
    handleLampEvent();
  }

  // Update DB polling rate, if needed
  if (millis() > time_now + UPDATE_INTERVAL) {
    time_now = millis();
    INTERVAL = db.getConfig();
  }
}

void handleButtonEvent() {
  // When button pushed, updated DB to trigger other node lamp event
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

void handleLampEvent() {
  // Turn on the LED strip when the other user pushes their button
  if (db.getPartnerNodeRecord()) {
    lamp.enableFullLED();
  }
  lamp.disableLED();
}

void checkWifiStatus() {
  // If WIFI connection is lost then display LED error lighting
  if (WiFi.status() == WL_CONNECTED && !wifi_connected) {
    wifi_connected = true;
    lamp.success();
    lamp.disableLED();
  }
  while (WiFi.status() != WL_CONNECTED || WiFi.status() == WL_CONNECTION_LOST || WiFi.status() == WL_DISCONNECTED || WiFi.status() == WL_CONNECT_FAILED || WiFi.status() == WL_IDLE_STATUS) {
    if (millis() > time_now + INTERVAL) {
      time_now = millis();
      wifi_connected = false;
      lamp.error();
      lamp.disableLED();
    }
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
  // When not connected to WIFI, the LED will pulse
  if (millis() > time_now + INTERVAL) {
    time_now = millis();
    lamp.waiting();
  }
}
