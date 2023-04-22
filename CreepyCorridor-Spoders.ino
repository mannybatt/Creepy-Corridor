



// ***************************************
// ********** Global Variables ***********
// ***************************************


//Globals for Wifi Setup and OTA
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//WiFi Credentials
#ifndef STASSID
#define STASSID "Frostmourne"
#define STAPSK "warproom"
#endif
const char* ssid = STASSID;
const char* password = STAPSK;

//Globals for MQTT
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#define MQTT_CONN_KEEPALIVE 300
#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME "atomisk966"
#define AIO_KEY "e0c3d4198f7c42c6843cc67d87c9629d"

//FastLED
#include <FastLED.h>
#define LED_PIN D8
#define NUM_LEDS 8
#define BRIGHTNESS 255
#define LED_TYPE WS2811
#define COLOR_ORDER RGB
CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 100

//Initialize and Subscribe to MQTT
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe creepyMode = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/creepy-corridor.creepymode");

//Input/Output
#define audioTriggerRelay D5  //For some fucking reason, 5V relays use opposite HIGH/HIGH logic, just go with it
#define volumeUpRelay D6
#define auxChangeRelay D7
#define funnyBonezRelay D3
#define speakerPowerRelay D4
#define syncWithFoggy D2

//Variables
int operationalMode = 1;
byte r = 0;
uint16_t simulateTrigger = 0;
int desiredBrightness = 254;
float mqttConnectFlag = 0.0;


// ***************************************
// *************** Setup *****************
// ***************************************

void setup() {

  //Relay Setup
  pinMode(audioTriggerRelay, OUTPUT);
  pinMode(volumeUpRelay, OUTPUT);
  pinMode(auxChangeRelay, OUTPUT);
  pinMode(funnyBonezRelay, OUTPUT);
  pinMode(speakerPowerRelay, OUTPUT);
  pinMode(syncWithFoggy, INPUT_PULLUP);
  digitalWrite(audioTriggerRelay, HIGH);
  digitalWrite(volumeUpRelay, HIGH);
  digitalWrite(auxChangeRelay, HIGH);
  digitalWrite(funnyBonezRelay, HIGH);
  digitalWrite(speakerPowerRelay, HIGH);

  //Initialize Serial, WiFi, & OTA
  wifiSetup();

  //Initialize RGB
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 900);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  delay(1000);
  spiderLight(1);
 /*
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i == 0) {
      setPixel(i, 0, 0, 0);
    }
    else {
      setPixel(i, 255, 0, 0);
    }
    delay(100);
    FastLED.show();
    Serial.println(i);
  }
  FastLED.show();
  */

  //Relay Test
  bootRelays();

  

  //Initialize MQTT
  mqtt.subscribe(&creepyMode);

  //All set
  delay(3000);
  spiderLight(0);
  Serial.println("Setup Complete!");
}





// ***************************************
// ************* Da Loop *****************
// ***************************************


void loop() {

  //Serial.println("LOOP");

  //OTA & MQTT
  ArduinoOTA.handle();
  MQTT_connect();

  //Recieve MQTT
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(0.01))) {
    uint16_t valueOperational = atoi((char *)creepyMode.lastread);
    Serial.println(valueOperational);
    if (valueOperational == 1 || valueOperational == 2) {
      operationalMode = valueOperational;
    }
    else if (valueOperational == 104) {
      simulateTrigger = 1;
    }
    else {}
    delay(10);
  }

  //State Manager
  if (operationalMode == 2) {  // No Music Display mode
    Serial.println("Mode 2");
    spiderLight(1);
  }

  else if (operationalMode == 1) {  // Standard Operation
    // 1. Listen for sync with Foggy
    // 2. Light up spider eyes
    // 3. Play Spider attack sound after delay
    // 4. activate funny bonez relay


    int syncCheck = digitalRead(syncWithFoggy);
    if (syncCheck == 0 || simulateTrigger == 1) {
      Serial.println("[Begin Spoder Performance]");
      funnyBonez();
      delay(2000);
      spiderLight(1);
      delay(1500);
      playSpiderAttack();
      delay(30000);
      spiderLight(0);
      Serial.println("[End Spoder Performance]");
      simulateTrigger = 0;
    }
  }

  delay(1);
}




// ***************************************
// ********** Backbone Methods ***********
// ***************************************


void spiderLight(int mode) {

  //Fade in to desired brightness
  if (mode == 1) {
    r = 0;
    while (r < desiredBrightness) {
      for (int i = 1; i < NUM_LEDS; i++) {
        setPixel(i, r, 0, 0);
      }
      delay(3);
      FastLED.show();
      r++;
    }
  }

  //Fade out to Darkness
  else if (mode == 0) {
    r = desiredBrightness;
    while (r != 0) {
      for (int i = 1; i < NUM_LEDS; i++) {
        setPixel(i, r, 0, 0);
      }
      delay(3);
      FastLED.show();
      r--;
    }
  }
  Serial.println("SpiderLight Done");
}

void playSpiderAttack() {

  digitalWrite(audioTriggerRelay, LOW);
  delay(200);
  digitalWrite(audioTriggerRelay, HIGH);
}

void funnyBonez() {

  digitalWrite(funnyBonezRelay, LOW);
  delay(200);
  digitalWrite(funnyBonezRelay, HIGH);
}

void bootRelays() {

  delay(250);
  digitalWrite(speakerPowerRelay, LOW);
  delay(3000);

  digitalWrite(volumeUpRelay, LOW);
  delay(200);
  digitalWrite(volumeUpRelay, HIGH);
  delay(200);
  digitalWrite(volumeUpRelay, LOW);
  delay(200);
  digitalWrite(volumeUpRelay, HIGH);
  delay(200);
  digitalWrite(volumeUpRelay, LOW);
  delay(200);
  digitalWrite(volumeUpRelay, HIGH);
  delay(200);
  digitalWrite(volumeUpRelay, LOW);
  delay(200);
  digitalWrite(volumeUpRelay, HIGH);
  delay(200);
  digitalWrite(volumeUpRelay, LOW);
  delay(200);
  digitalWrite(volumeUpRelay, HIGH);
  delay(200);
  digitalWrite(volumeUpRelay, LOW);
  delay(200);
  digitalWrite(volumeUpRelay, HIGH);
  delay(200);
  digitalWrite(volumeUpRelay, LOW);
  delay(200);
  digitalWrite(volumeUpRelay, HIGH);
  delay(200);

  delay(500);
  digitalWrite(auxChangeRelay, LOW);
  delay(200);
  digitalWrite(auxChangeRelay, HIGH);

  delay(1000);
  digitalWrite(audioTriggerRelay, LOW);
  delay(250);
  digitalWrite(audioTriggerRelay, HIGH);
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
  leds[Pixel].r = red;
  leds[Pixel].g = green;
  leds[Pixel].b = blue;
}

void MQTT_connect() {

  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
    if (mqttConnectFlag == 0) {
      //Serial.println("Connected");
      mqttConnectFlag++;
    }
    return;
  }
  Serial.println("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      //while (1);
      Serial.println("Wait 5 secomds to reconnect");
      delay(5000);
    }
  }
}

void wifiSetup() {

  //Serial
  Serial.begin(115200);
  delay(300);
  Serial.println();
  Serial.println();
  Serial.println("****************************************");
  Serial.println("Booting");

  //WiFi and OTA
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  ArduinoOTA.setHostname("CreepyCorridor-Spoders");                                                          /** TODO **/
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
