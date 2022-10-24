


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
#define STAPSK  "warproom"
#endif
const char* ssid = STASSID;
const char* password = STAPSK;

//Globals for MQTT
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#define MQTT_CONN_KEEPALIVE 300
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "atomisk966"
#define AIO_KEY         "e0c3d4198f7c42c6843cc67d87c9629d"

//FastLED
#include <FastLED.h>
#define LED_PIN     D8
#define NUM_LEDS    12
#define BRIGHTNESS  255
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 100

//Initialize and Subscribe to MQTT
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe creepyMode = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/creepy-corridor.creepymode");
//Adafruit_MQTT_Subscribe foggyMode = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/creepy-corridor.foggy");
//Adafruit_MQTT_Subscribe simulateSensor = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/creepy-corridor.simulate");

//MP3 Player
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>
DFRobotDFPlayerMini myDFPlayer;
SoftwareSerial mySoftwareSerial(D4, D2);  //Pins for MP3 Player Serial (RX, TX)

//Input/Output
#define strobeRelay D1    //For some fucking reason, 5V relays use opposite HIGH/HIGH logic, just go with it
#define syncWithSpoderRelay D3
#define fogMachineOnRelay D7
#define fogMachineOffRelay D6
#define sensor D5

//Variables
int operationalMode = 1;
int beginFog = 0;
int fogIsRunning = 0;
int fogDuration = 8000;
uint16_t valueFoggy = 0;
uint16_t simulateTrigger = 0;
unsigned long previousMillis = 0;
unsigned long currentMillis;
unsigned long previousTime;
char songSelection = 1;
byte b = 0;
float mqttConnectFlag = 0.0;




// ***************************************
// *************** Setup *****************
// ***************************************


void setup() {

  //Relay Setup
  pinMode(strobeRelay, OUTPUT);
  pinMode(syncWithSpoderRelay, OUTPUT);
  pinMode(fogMachineOnRelay, OUTPUT);
  pinMode(fogMachineOffRelay, OUTPUT);
  pinMode(sensor, INPUT);
  digitalWrite(strobeRelay, HIGH);
  digitalWrite(syncWithSpoderRelay, HIGH);
  digitalWrite(fogMachineOnRelay, LOW);          //3.3v Relays with normal logic
  digitalWrite(fogMachineOffRelay, LOW);         //3.3v Relays with normal logic

  //Initialize RGB
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 900);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);
  delay(1000);
  for (int i = 0; i < NUM_LEDS; i++) {
    setPixel(i, 0, 0, 255);
    FastLED.show();
  }
  delay(10);
  FastLED.show();

  //Initialize Serial, WiFi, & OTA
  wifiSetup();

  //Initialize MQTT
  mqtt.subscribe(&creepyMode);
  //mqtt.subscribe(&foggyMode);
  //mqtt.subscribe(&simulateSensor);
  MQTT_connect();

  //MP3
  mySoftwareSerial.begin(9600);
  delay(1000);
  myDFPlayer.begin(mySoftwareSerial);
  Serial.println();
  Serial.println("DFPlayer initialized!");
  myDFPlayer.setTimeOut(500); //Timeout serial 500ms
  myDFPlayer.volume(0); //Volume 0-30
  myDFPlayer.EQ(DFPLAYER_EQ_ROCK); //Equalization normal
  delay(1000);
  myDFPlayer.volume(30);
  myDFPlayer.play(2);
  delay(5000);
  myDFPlayer.stop();

  //Test Strobe
  digitalWrite(strobeRelay, LOW);
  delay(3500);
  digitalWrite(strobeRelay, HIGH);
  delay(250);

  //All set
  FastLED.clear();
  FastLED.show();
  delay(50);
  FastLED.show();
  Serial.println("Setup Complete!");
  delay(3000);
}




// ***************************************
// ************* Da Loop *****************
// ***************************************


void loop() {

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
    else if(valueOperational == 11){
      beginFog = 1;
    }
    else if(valueOperational == 103){
      simulateTrigger = 1;
    }
    else{}
    delay(10);
  }

  //FOG TIME BABY
  if (beginFog == 1 && operationalMode == 1) {
    Serial.println("FOG ON");
    digitalWrite(fogMachineOnRelay, HIGH);
    delay(300);
    digitalWrite(fogMachineOnRelay, LOW);
    previousTime = millis();
    beginFog = 0;
    fogIsRunning = 1;
  }

  //State Manager
  if (operationalMode == 2) {                             //Sensorless Operation
    Serial.println("Mode 2");
    for (int i = 0; i < NUM_LEDS; i++) {
      setPixel(i, 0, 0, 255);
    }
    delay(10);
    FastLED.show();
  }
  
  else if (operationalMode == 1) {                        //Standard Operation
    // 1. Sense for peeps or MQTT Trigger
    // 2. Close Sync relay with Spoders
    // 3. play thunder/hobbit sounds
    // 4. Flash Strobe relay
    // 5. Blue glow on light bar

    FastLED.clear();
    FastLED.show();
    int sensorReading = digitalRead(sensor);
    if (sensorReading == HIGH || simulateTrigger == 1) {
      delay(100); //Prevents false readings
      int sensorReading2 = digitalRead(sensor);
      if (sensorReading2 == HIGH || simulateTrigger == 1) {

        Serial.println("[Begin Foggy Performance]");

        // ** SYNC WITH SPODER BRAIN **
        Serial.println("**SYNC");
        digitalWrite(syncWithSpoderRelay, LOW);
        delay(100);
        digitalWrite(syncWithSpoderRelay, HIGH);

        // ** THUNDER & SPIDERS **
        Serial.println("**THUNDER AND SPIDERS**");
        songSelection = 1;
        myDFPlayer.play(songSelection);

        // ** STROBE **
        Serial.println("**STROBE**");
        digitalWrite(strobeRelay, LOW);
        //Delay must be broken up to check for foggy
        int playTime = 7;
        for (int i = 0; i < playTime; i++) {
          delay(500);

          if (fogIsRunning == 1) {
            unsigned long currentTime = millis();
            Serial.print("time: ");
            Serial.println(currentTime - previousTime);
            if ((currentTime - previousTime) > fogDuration) {
              previousTime = currentTime;
              fogIsRunning = 0;
              digitalWrite(fogMachineOffRelay, HIGH);
              delay(300);
              digitalWrite(fogMachineOffRelay, LOW);
              Serial.println("FOG OFF");
            }
          }
        }
        digitalWrite(strobeRelay, HIGH);

        // ** LEDS **
        Serial.println("**leds");
        b = 0;
        while (b < 255) {
          for (int i = 0; i < NUM_LEDS; i++) {
            setPixel(i, 0, 0, b);
          }
          b++;
          delay(5);
          FastLED.show();
        }
        //Delay must be broken up to check for foggy
        playTime = 108;
        for (int i = 0; i < playTime; i++) {
          delay(500);

          if (fogIsRunning == 1) {
            unsigned long currentTime = millis();
            Serial.print("time: ");
            Serial.println(currentTime - previousTime);
            if ((currentTime - previousTime) > fogDuration) {
              previousTime = currentTime;
              fogIsRunning = 0;
              digitalWrite(fogMachineOffRelay, HIGH);
              delay(300);
              digitalWrite(fogMachineOffRelay, LOW);
              Serial.println("FOG OFF");
            }
          }
        }
        while (b > 0) {
          for (int i = 0; i < NUM_LEDS; i++) {
            setPixel(i, 0, 0, b);
          }
          b--;
          delay(5);
          FastLED.show();
        }
        simulateTrigger = 0;
      }
      Serial.println("[End Foggy Performance]");
    }
  }

  //Fog Timer
  if (fogIsRunning == 1) {
    unsigned long currentTime = millis();
    Serial.print("time: ");
    Serial.println(currentTime - previousTime);
    if ((currentTime - previousTime) > fogDuration) {
      previousTime = currentTime;
      fogIsRunning = 0;
      digitalWrite(fogMachineOffRelay, HIGH);
      delay(300);
      digitalWrite(fogMachineOffRelay, LOW);
      Serial.println("FOG OFF");
    }
  }
  delay(1);
}




// ***************************************
// ********** Backbone Methods ***********
// ***************************************


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
  ArduinoOTA.setHostname("CreepyCorridor-Foggy");                                                          /** TODO **/
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
