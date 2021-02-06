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
#define AP_NAME "My Wifi Lamp"

// FIREBASE RTDB Information
// Add firebase information here
String FIREBASE_HOST = "<firebase-name>.firebaseio.com";
String FIREBASE_AUTH = "<database-secret>";

// Swap these fields for each lamp.
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
WiFiManager wifiManager;

// Recheck DB interval
int INTERVAL                = 1000;
int RESET_INTERVAL          = 10000;
int UPDATE_INTERVAL         = 60000;
unsigned long main_timer    = 0;
unsigned long config_timer  = 0;
unsigned long button_timer  = 0;
unsigned long button_pressed_timer = 0;

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
  if (millis() >= main_timer + INTERVAL) {
    main_timer = millis();
    handleLampEvent();
  }

  // Update DB polling rate, if needed
  if (millis() >= config_timer + UPDATE_INTERVAL) {
    config_timer = millis();
    INTERVAL = db.getConfig();
  }
}

void handleButtonEvent()
{
  if (btn.isPushed()) {
    // When button pushed, updated DB to trigger other node lamp event
    if (db.setCurrentNodeRecord(true)) {
      reset_btn_state = true;
      lamp.enableSingleLED();
      delay(1000);
    }

    // If button pressed for x seconds
    // reset the AP state
    if (button_pressed_timer == 0) {
      button_pressed_timer = millis();
    }

    if (millis() >= button_pressed_timer + button_timer + RESET_INTERVAL) {
      button_timer = millis();
      resetWifiConfig();
    }
  }

  // If button released
  // reset the state of the database for this node
  if (!btn.isPushed() && reset_btn_state) {
    reset_btn_state = false;
    lamp.disableLED();
    db.setCurrentNodeRecord(false);
    button_pressed_timer = 0;
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
    if (millis() > main_timer + INTERVAL) {
      main_timer = millis();
      wifi_connected = false;
      lamp.error();
      lamp.disableLED();
    }
  }
}

void startWifiManager() {
  // Wifi manager will connect to a pre-existing WIFI network
  // If no WIFI network is found the device will become an AP to set/find a new WIFI network

  wifiManager.autoConnect(AP_NAME);
}

void resetWifiConfig() {
  Serial.println("Resetting AP configuration");
  delay(1000);
  wifiManager.resetSettings();
  lamp.waiting();
  wifiManager.startConfigPortal(AP_NAME);
}
