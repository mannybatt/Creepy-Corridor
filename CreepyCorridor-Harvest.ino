







// ***************************************
// ********** Global Variables ***********
// ***************************************

//Globals fo Wifi Setup and OTA
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
#define LED_PIN     D6
#define NUM_LEDS    132
#define BRIGHTNESS  255
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 100

//Initialize and Subscribe to MQTT
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe creepyMode = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/creepy-corridor.creepymode");

//Input/Output
#define sensor D7
#define laughingSoundRelay D4
#define forestSoundRelay D2

//Variables
int operationalMode = 1;
uint16_t simulateTrigger = 0;
float mqttConnectFlag = 0.0;
const int numberOfPumpkins = 23;
int chosenPumpkins[numberOfPumpkins] = {4, 14, 19, 21, 23, 17, 8, 15, 10, 11, 9, 6, 5, 7, 3, 1, 16, 18, 22, 2, 13, 20, 12};

int resetRandomArray = 1;


// ***************************************
// *************** Setup *****************
// ***************************************


void setup() {

  //Relay Setup
  pinMode(laughingSoundRelay, OUTPUT);
  pinMode(forestSoundRelay, OUTPUT);
  digitalWrite(laughingSoundRelay, LOW);
  digitalWrite(forestSoundRelay, LOW);

  //Initialize RGB
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 800);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);

  //Pumpkin LEDs Boot
  pumpkinBoot(20);
  delay(500);
  FastLED.clear();
  delay(10);
  FastLED.show();
  FastLED.show();

  //Relay Test
  //forestSound();
  delay(500);
  //laughingSound();

  //Initialize Serial, WiFi, & OTA
  wifiSetup();

  //Initialize MQTT
  mqtt.subscribe(&creepyMode);
  MQTT_connect();

  //All set
  setPumpkins();
  Serial.println("Setup Complete!");
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
    else if (valueOperational == 102) {
      simulateTrigger = 1;
    }
    else {}
    delay(1);
  }

  //State Manager
  if (operationalMode == 2) {
    Serial.println("Mode 2");

    //delay(66000);
  }

  else if (operationalMode == 1) {

    // TODO
    // 0. All On
    // 1. Sense peeps
    // 2. hard off/laughter
    // 3. darkness for two seconds for lil boo
    // 4. Fade In and stay on until restart




    /*
      for (int i = 0; i < numberOfPumpkins; i++) {
      pumpkinLights(chosenPumpkins[i]);
      //delayMicroseconds(1200);
      }
      pumpkinLights(numberOfPumpkins + 1);
    */


    int sensorReading = digitalRead(sensor);
    if (sensorReading == HIGH || simulateTrigger == 1) {
      delay(100); //Prevents false readings
      int sensorReading2 = digitalRead(sensor);
      if (sensorReading2 == HIGH || simulateTrigger == 1) {
        delay(100);
        int sensorReading3 = digitalRead(sensor);
        if (sensorReading3 == HIGH || simulateTrigger == 1) {
          Serial.println("[Begin Harvest Performance]");

          //** FOREST
          Serial.println("**FOREST");
          forestSound();

          delay(400);

          //** LAUGHING
          Serial.println("**LAUGH");
          laughingSound();

          delay(600); //400

          //** PUMPKINS Off
          Serial.println("**OFF");
          //pumpkinsDim();
          FastLED.clear();
          FastLED.show();
          FastLED.show();
          bigHeadEnding();

          delay(2000);

          for (int i = 0; i < numberOfPumpkins; i++) {
            pumpkinLights(chosenPumpkins[i]);
            //delayMicroseconds(3200);
            delay(35);
          }
          pumpkinLights(numberOfPumpkins + 1);

          //pumpkinsDim();
          /*

          for (int i = 0; i < 20; i++) {
            glowDown();
            glowUp();
            delay(500);
          }
          */


          delay(25000);
          Serial.println("[End Harvest Performance]");
        }
      }
      simulateTrigger = 0;
    }
  }
  delay(1);
}
 



// ***************************************
// ********** Backbone Methods ***********
// ***************************************


void glowUp() {

  int dimming = 50;
  while (dimming != 255) {

    setPixel(0, dimming, 0, 0);
    setPixel(1, dimming, 0, 0);
    setPixel(2, dimming, 0, 0);
    setPixel(3, dimming, 0, 0);
    setPixel(4, dimming, 0, 0);
    setPixel(5, dimming, 0, 0);
    setPixel(6, dimming, 0, 0);
    setPixel(7, dimming, 0, 0);
    setPixel(8, dimming, 0, 0);
    setPixel(9, dimming, 0, 0);
    setPixel(10, dimming, 0, 0);
    setPixel(11, dimming, 0, 0);
    setPixel(12, dimming, 0, 0);
    setPixel(13, dimming, 0, 0);
    setPixel(14, dimming, 0, 0);
    setPixel(15, dimming, 0, 0);
    setPixel(16, dimming, 0, 0);
    setPixel(17, dimming, 0, 0);
    setPixel(18, dimming, 0, 0);
    setPixel(19, dimming, 0, 0);
    setPixel(20, dimming, 0, 0);
    setPixel(21, dimming, 0, 0);
    setPixel(22, dimming, 0, 0);
    setPixel(23, dimming, 0, 0);
    setPixel(24, dimming, 0, 0);
    setPixel(25, dimming, 0, 0);
    setPixel(26, dimming, 0, 0);
    setPixel(27, dimming, 0, 0);
    setPixel(28, dimming, 0, 0);
    setPixel(29, dimming, 0, 0);
    setPixel(30, dimming, 0, 0);
    setPixel(31, dimming, 0, 0);
    setPixel(32, dimming, 0, 0);
    setPixel(33, dimming, 0, 0);
    setPixel(34, dimming, 0, 0);
    setPixel(35, dimming, 0, 0);
    setPixel(36, dimming, 0, 0);
    setPixel(37, 0, dimming, 0);
    setPixel(38, 0, dimming, 0);
    setPixel(39, 0, dimming, 0);
    setPixel(40, 0, dimming, 0);
    setPixel(41, 0, dimming, 0);
    setPixel(42, dimming, 0, 0);
    setPixel(43, dimming, 0, 0);
    setPixel(44, dimming, 0, 0);
    setPixel(45, dimming, 0, 0);
    setPixel(46, dimming, 0, 0);
    setPixel(47, dimming, 0, 0);
    setPixel(48, dimming, 0, 0);
    setPixel(49, dimming, 0, 0);
    setPixel(50, dimming, 0, 0);
    setPixel(51, dimming, 0, 0);
    setPixel(52, 0, dimming, 0);
    setPixel(53, 0, dimming, 0);
    setPixel(54, 0, dimming, 0);
    setPixel(55, 0, dimming, 0);
    setPixel(56, 0, dimming, 0);
    setPixel(57, dimming, 0, 0);
    setPixel(58, dimming, 0, 0);
    setPixel(59, dimming, 0, 0);
    setPixel(60, dimming, 0, 0);
    setPixel(61, dimming, 0, 0);
    setPixel(62, dimming, 0, 0);
    setPixel(63, dimming, 0, 0);
    setPixel(64, dimming, 0, 0);
    setPixel(65, dimming, 0, 0);
    setPixel(66, dimming, 0, 0);
    setPixel(67, dimming, 0, 0);
    setPixel(68, dimming, 0, 0);
    setPixel(69, dimming, 0, 0);
    setPixel(70, dimming, 0, 0);
    setPixel(71, dimming, 0, 0);
    setPixel(72, dimming, 0, 0);
    setPixel(73, dimming, 0, 0);
    setPixel(74, dimming, 0, 0);
    setPixel(75, dimming, 0, 0);
    setPixel(76, dimming, 0, 0);
    setPixel(77, 0, dimming, 0);
    setPixel(78, 0, dimming, 0);
    setPixel(79, 0, dimming, 0);
    setPixel(80, 0, dimming, 0);
    setPixel(81, 0, dimming, 0);
    setPixel(82, dimming, 0, 0);
    setPixel(83, dimming, 0, 0);
    setPixel(84, dimming, 0, 0);
    setPixel(85, dimming, 0, 0);
    setPixel(86, dimming, 0, 0);
    setPixel(87, dimming, 0, 0);
    setPixel(88, dimming, 0, 0);
    setPixel(89, dimming, 0, 0);
    setPixel(90, dimming, 0, 0);
    setPixel(91, dimming, 0, 0);
    setPixel(92, dimming, 0, 0);
    setPixel(93, dimming, 0, 0);
    setPixel(94, dimming, 0, 0);
    setPixel(95, dimming, 0, 0);
    setPixel(96, dimming, 0, 0);
    setPixel(97, 0, dimming, 0);
    setPixel(98, 0, dimming, 0);
    setPixel(99, 0, dimming, 0);
    setPixel(100, 0, dimming, 0);
    setPixel(101, dimming, 0, 0);
    setPixel(102, dimming, 0, 0);
    setPixel(103, dimming, 0, 0);
    setPixel(104, dimming, 0, 0);
    setPixel(105, dimming, 0, 0);
    setPixel(106, dimming, 0, 0);
    setPixel(107, dimming, 0, 0);
    setPixel(108, dimming, 0, 0);
    setPixel(109, dimming, 0, 0);
    setPixel(110, dimming, 0, 0);
    setPixel(111, dimming, 0, dimming);
    setPixel(112, dimming, 0, dimming);
    setPixel(113, dimming, 0, dimming);
    setPixel(114, dimming, 0, dimming);
    setPixel(115, dimming, 0, dimming);
    setPixel(116, dimming, 0, dimming);
    setPixel(117, dimming, 0, dimming);
    setPixel(118, dimming, 0, dimming);
    setPixel(119, dimming, 0, dimming);
    setPixel(120, dimming, 0, dimming);
    setPixel(121, dimming, 0, dimming);
    setPixel(122, dimming, 0, dimming);
    setPixel(123, dimming, 0, dimming);
    setPixel(124, dimming, 0, dimming);
    setPixel(125, dimming, 0, dimming);
    setPixel(126, dimming, 0, dimming);
    setPixel(127, dimming, 0, dimming);
    setPixel(128, dimming, 0, dimming);
    setPixel(129, dimming, 0, dimming);
    setPixel(130, dimming, 0, dimming);
    setPixel(131, dimming, 0, dimming);
    setPixel(132, dimming, 0, dimming);

    delay(100);

    FastLED.show();
    FastLED.show();
    dimming++;
  }
}

void glowDown() {

  int dimming = 255;
  while (dimming != 50) {

    setPixel(0, dimming, 0, 0);
    setPixel(1, dimming, 0, 0);
    setPixel(2, dimming, 0, 0);
    setPixel(3, dimming, 0, 0);
    setPixel(4, dimming, 0, 0);
    setPixel(5, dimming, 0, 0);
    setPixel(6, dimming, 0, 0);
    setPixel(7, dimming, 0, 0);
    setPixel(8, dimming, 0, 0);
    setPixel(9, dimming, 0, 0);
    setPixel(10, dimming, 0, 0);
    setPixel(11, dimming, 0, 0);
    setPixel(12, dimming, 0, 0);
    setPixel(13, dimming, 0, 0);
    setPixel(14, dimming, 0, 0);
    setPixel(15, dimming, 0, 0);
    setPixel(16, dimming, 0, 0);
    setPixel(17, dimming, 0, 0);
    setPixel(18, dimming, 0, 0);
    setPixel(19, dimming, 0, 0);
    setPixel(20, dimming, 0, 0);
    setPixel(21, dimming, 0, 0);
    setPixel(22, dimming, 0, 0);
    setPixel(23, dimming, 0, 0);
    setPixel(24, dimming, 0, 0);
    setPixel(25, dimming, 0, 0);
    setPixel(26, dimming, 0, 0);
    setPixel(27, dimming, 0, 0);
    setPixel(28, dimming, 0, 0);
    setPixel(29, dimming, 0, 0);
    setPixel(30, dimming, 0, 0);
    setPixel(31, dimming, 0, 0);
    setPixel(32, dimming, 0, 0);
    setPixel(33, dimming, 0, 0);
    setPixel(34, dimming, 0, 0);
    setPixel(35, dimming, 0, 0);
    setPixel(36, dimming, 0, 0);
    setPixel(37, 0, dimming, 0);
    setPixel(38, 0, dimming, 0);
    setPixel(39, 0, dimming, 0);
    setPixel(40, 0, dimming, 0);
    setPixel(41, 0, dimming, 0);
    setPixel(42, dimming, 0, 0);
    setPixel(43, dimming, 0, 0);
    setPixel(44, dimming, 0, 0);
    setPixel(45, dimming, 0, 0);
    setPixel(46, dimming, 0, 0);
    setPixel(47, dimming, 0, 0);
    setPixel(48, dimming, 0, 0);
    setPixel(49, dimming, 0, 0);
    setPixel(50, dimming, 0, 0);
    setPixel(51, dimming, 0, 0);
    setPixel(52, 0, dimming, 0);
    setPixel(53, 0, dimming, 0);
    setPixel(54, 0, dimming, 0);
    setPixel(55, 0, dimming, 0);
    setPixel(56, 0, dimming, 0);
    setPixel(57, dimming, 0, 0);
    setPixel(58, dimming, 0, 0);
    setPixel(59, dimming, 0, 0);
    setPixel(60, dimming, 0, 0);
    setPixel(61, dimming, 0, 0);
    setPixel(62, dimming, 0, 0);
    setPixel(63, dimming, 0, 0);
    setPixel(64, dimming, 0, 0);
    setPixel(65, dimming, 0, 0);
    setPixel(66, dimming, 0, 0);
    setPixel(67, dimming, 0, 0);
    setPixel(68, dimming, 0, 0);
    setPixel(69, dimming, 0, 0);
    setPixel(70, dimming, 0, 0);
    setPixel(71, dimming, 0, 0);
    setPixel(72, dimming, 0, 0);
    setPixel(73, dimming, 0, 0);
    setPixel(74, dimming, 0, 0);
    setPixel(75, dimming, 0, 0);
    setPixel(76, dimming, 0, 0);
    setPixel(77, 0, dimming, 0);
    setPixel(78, 0, dimming, 0);
    setPixel(79, 0, dimming, 0);
    setPixel(80, 0, dimming, 0);
    setPixel(81, 0, dimming, 0);
    setPixel(82, dimming, 0, 0);
    setPixel(83, dimming, 0, 0);
    setPixel(84, dimming, 0, 0);
    setPixel(85, dimming, 0, 0);
    setPixel(86, dimming, 0, 0);
    setPixel(87, dimming, 0, 0);
    setPixel(88, dimming, 0, 0);
    setPixel(89, dimming, 0, 0);
    setPixel(90, dimming, 0, 0);
    setPixel(91, dimming, 0, 0);
    setPixel(92, dimming, 0, 0);
    setPixel(93, dimming, 0, 0);
    setPixel(94, dimming, 0, 0);
    setPixel(95, dimming, 0, 0);
    setPixel(96, dimming, 0, 0);
    setPixel(97, 0, dimming, 0);
    setPixel(98, 0, dimming, 0);
    setPixel(99, 0, dimming, 0);
    setPixel(100, 0, dimming, 0);
    setPixel(101, dimming, 0, 0);
    setPixel(102, dimming, 0, 0);
    setPixel(103, dimming, 0, 0);
    setPixel(104, dimming, 0, 0);
    setPixel(105, dimming, 0, 0);
    setPixel(106, dimming, 0, 0);
    setPixel(107, dimming, 0, 0);
    setPixel(108, dimming, 0, 0);
    setPixel(109, dimming, 0, 0);
    setPixel(110, dimming, 0, 0);
    setPixel(111, dimming, 0, dimming);
    setPixel(112, dimming, 0, dimming);
    setPixel(113, dimming, 0, dimming);
    setPixel(114, dimming, 0, dimming);
    setPixel(115, dimming, 0, dimming);
    setPixel(116, dimming, 0, dimming);
    setPixel(117, dimming, 0, dimming);
    setPixel(118, dimming, 0, dimming);
    setPixel(119, dimming, 0, dimming);
    setPixel(120, dimming, 0, dimming);
    setPixel(121, dimming, 0, dimming);
    setPixel(122, dimming, 0, dimming);
    setPixel(123, dimming, 0, dimming);
    setPixel(124, dimming, 0, dimming);
    setPixel(125, dimming, 0, dimming);
    setPixel(126, dimming, 0, dimming);
    setPixel(127, dimming, 0, dimming);
    setPixel(128, dimming, 0, dimming);
    setPixel(129, dimming, 0, dimming);
    setPixel(130, dimming, 0, dimming);
    setPixel(131, dimming, 0, dimming);
    setPixel(132, dimming, 0, dimming);

    delay(100);

    FastLED.show();
    FastLED.show(); 
    dimming--;
  }
}

void pumpkinsDim() {

  int dimming = 0;
  while (dimming != 255) {

    setPixel(0, dimming, 0, 0);
    setPixel(1, dimming, 0, 0);
    setPixel(2, dimming, 0, 0);
    setPixel(3, dimming, 0, 0);
    setPixel(4, dimming, 0, 0);
    setPixel(5, dimming, 0, 0);
    setPixel(6, dimming, 0, 0);
    setPixel(7, dimming, 0, 0);
    setPixel(8, dimming, 0, 0);
    setPixel(9, dimming, 0, 0);
    setPixel(10, dimming, 0, 0);
    setPixel(11, dimming, 0, 0);
    setPixel(12, dimming, 0, 0);
    setPixel(13, dimming, 0, 0);
    setPixel(14, dimming, 0, 0);
    setPixel(15, dimming, 0, 0);
    setPixel(16, dimming, 0, 0);
    setPixel(17, dimming, 0, 0);
    setPixel(18, dimming, 0, 0);
    setPixel(19, dimming, 0, 0);
    setPixel(20, dimming, 0, 0);
    setPixel(21, dimming, 0, 0);
    setPixel(22, dimming, 0, 0);
    setPixel(23, dimming, 0, 0);
    setPixel(24, dimming, 0, 0);
    setPixel(25, dimming, 0, 0);
    setPixel(26, dimming, 0, 0);
    setPixel(27, dimming, 0, 0);
    setPixel(28, dimming, 0, 0);
    setPixel(29, dimming, 0, 0);
    setPixel(30, dimming, 0, 0);
    setPixel(31, dimming, 0, 0);
    setPixel(32, dimming, 0, 0);
    setPixel(33, dimming, 0, 0);
    setPixel(34, dimming, 0, 0);
    setPixel(35, dimming, 0, 0);
    setPixel(36, dimming, 0, 0);
    setPixel(37, 0, dimming, 0);
    setPixel(38, 0, dimming, 0);
    setPixel(39, 0, dimming, 0);
    setPixel(40, 0, dimming, 0);
    setPixel(41, 0, dimming, 0);
    setPixel(42, dimming, 0, 0);
    setPixel(43, dimming, 0, 0);
    setPixel(44, dimming, 0, 0);
    setPixel(45, dimming, 0, 0);
    setPixel(46, dimming, 0, 0);
    setPixel(47, dimming, 0, 0);
    setPixel(48, dimming, 0, 0);
    setPixel(49, dimming, 0, 0);
    setPixel(50, dimming, 0, 0);
    setPixel(51, dimming, 0, 0);
    setPixel(52, 0, dimming, 0);
    setPixel(53, 0, dimming, 0);
    setPixel(54, 0, dimming, 0);
    setPixel(55, 0, dimming, 0);
    setPixel(56, 0, dimming, 0);
    setPixel(57, dimming, 0, 0);
    setPixel(58, dimming, 0, 0);
    setPixel(59, dimming, 0, 0);
    setPixel(60, dimming, 0, 0);
    setPixel(61, dimming, 0, 0);
    setPixel(62, dimming, 0, 0);
    setPixel(63, dimming, 0, 0);
    setPixel(64, dimming, 0, 0);
    setPixel(65, dimming, 0, 0);
    setPixel(66, dimming, 0, 0);
    setPixel(67, dimming, 0, 0);
    setPixel(68, dimming, 0, 0);
    setPixel(69, dimming, 0, 0);
    setPixel(70, dimming, 0, 0);
    setPixel(71, dimming, 0, 0);
    setPixel(72, dimming, 0, 0);
    setPixel(73, dimming, 0, 0);
    setPixel(74, dimming, 0, 0);
    setPixel(75, dimming, 0, 0);
    setPixel(76, dimming, 0, 0);
    setPixel(77, 0, dimming, 0);
    setPixel(78, 0, dimming, 0);
    setPixel(79, 0, dimming, 0);
    setPixel(80, 0, dimming, 0);
    setPixel(81, 0, dimming, 0);
    setPixel(82, dimming, 0, 0);
    setPixel(83, dimming, 0, 0);
    setPixel(84, dimming, 0, 0);
    setPixel(85, dimming, 0, 0);
    setPixel(86, dimming, 0, 0);
    setPixel(87, dimming, 0, 0);
    setPixel(88, dimming, 0, 0);
    setPixel(89, dimming, 0, 0);
    setPixel(90, dimming, 0, 0);
    setPixel(91, dimming, 0, 0);
    setPixel(92, dimming, 0, 0);
    setPixel(93, dimming, 0, 0);
    setPixel(94, dimming, 0, 0);
    setPixel(95, dimming, 0, 0);
    setPixel(96, dimming, 0, 0);
    setPixel(97, 0, dimming, 0);
    setPixel(98, 0, dimming, 0);
    setPixel(99, 0, dimming, 0);
    setPixel(100, 0, dimming, 0);
    setPixel(101, dimming, 0, 0);
    setPixel(102, dimming, 0, 0);
    setPixel(103, dimming, 0, 0);
    setPixel(104, dimming, 0, 0);
    setPixel(105, dimming, 0, 0);
    setPixel(106, dimming, 0, 0);
    setPixel(107, dimming, 0, 0);
    setPixel(108, dimming, 0, 0);
    setPixel(109, dimming, 0, 0);
    setPixel(110, dimming, 0, 0);
    setPixel(111, dimming, 0, dimming);
    setPixel(112, dimming, 0, dimming);
    setPixel(113, dimming, 0, dimming);
    setPixel(114, dimming, 0, dimming);
    setPixel(115, dimming, 0, dimming);
    setPixel(116, dimming, 0, dimming);
    setPixel(117, dimming, 0, dimming);
    setPixel(118, dimming, 0, dimming);
    setPixel(119, dimming, 0, dimming);
    setPixel(120, dimming, 0, dimming);
    setPixel(121, dimming, 0, dimming);
    setPixel(122, dimming, 0, dimming);
    setPixel(123, dimming, 0, dimming);
    setPixel(124, dimming, 0, dimming);
    setPixel(125, dimming, 0, dimming);
    setPixel(126, dimming, 0, dimming);
    setPixel(127, dimming, 0, dimming);
    setPixel(128, dimming, 0, dimming);
    setPixel(129, dimming, 0, dimming);
    setPixel(130, dimming, 0, dimming);
    setPixel(131, dimming, 0, dimming);
    setPixel(132, dimming, 0, dimming);

    delay(60);

    if (dimming == 80) {

    }

    FastLED.show();
    dimming++;
  }
}

void setPumpkins() {
  setPixel(0, 255, 0, 0);
  setPixel(1, 255, 0, 0);
  setPixel(2, 255, 0, 0);
  setPixel(3, 255, 0, 0);
  setPixel(4, 255, 0, 0);
  setPixel(5, 255, 0, 0);
  setPixel(6, 255, 0, 0);
  setPixel(7, 255, 0, 0);
  setPixel(8, 255, 0, 0);
  setPixel(9, 255, 0, 0);
  setPixel(10, 255, 0, 0);
  setPixel(11, 255, 0, 0);
  setPixel(12, 255, 0, 0);
  setPixel(13, 255, 0, 0);
  setPixel(14, 255, 0, 0);
  setPixel(15, 255, 0, 0);
  setPixel(16, 255, 0, 0);
  setPixel(17, 255, 0, 0);
  setPixel(18, 255, 0, 0);
  setPixel(19, 255, 0, 0);
  setPixel(20, 255, 0, 0);
  setPixel(21, 255, 0, 0);
  setPixel(22, 255, 0, 0);
  setPixel(23, 255, 0, 0);
  setPixel(24, 255, 0, 0);
  setPixel(25, 255, 0, 0);
  setPixel(26, 255, 0, 0);
  setPixel(27, 255, 0, 0);
  setPixel(28, 255, 0, 0);
  setPixel(29, 255, 0, 0);
  setPixel(30, 255, 0, 0);
  setPixel(31, 255, 0, 0);
  setPixel(32, 255, 0, 0);
  setPixel(33, 255, 0, 0);
  setPixel(34, 255, 0, 0);
  setPixel(35, 255, 0, 0);
  setPixel(36, 255, 0, 0);
  setPixel(37, 0, 255, 0);
  setPixel(38, 0, 255, 0);
  setPixel(39, 0, 255, 0);
  setPixel(40, 0, 255, 0);
  setPixel(41, 0, 255, 0);
  setPixel(42, 255, 0, 0);
  setPixel(43, 255, 0, 0);
  setPixel(44, 255, 0, 0);
  setPixel(45, 255, 0, 0);
  setPixel(46, 255, 0, 0);
  setPixel(47, 255, 0, 0);
  setPixel(48, 255, 0, 0);
  setPixel(49, 255, 0, 0);
  setPixel(50, 255, 0, 0);
  setPixel(51, 255, 0, 0);
  setPixel(52, 0, 255, 0);
  setPixel(53, 0, 255, 0);
  setPixel(54, 0, 255, 0);
  setPixel(55, 0, 255, 0);
  setPixel(56, 0, 255, 0);
  setPixel(57, 255, 0, 0);
  setPixel(58, 255, 0, 0);
  setPixel(59, 255, 0, 0);
  setPixel(60, 255, 0, 0);
  setPixel(61, 255, 0, 0);
  setPixel(62, 255, 0, 0);
  setPixel(63, 255, 0, 0);
  setPixel(64, 255, 0, 0);
  setPixel(65, 255, 0, 0);
  setPixel(66, 255, 0, 0);
  setPixel(67, 255, 0, 0);
  setPixel(68, 255, 0, 0);
  setPixel(69, 255, 0, 0);
  setPixel(70, 255, 0, 0);
  setPixel(71, 255, 0, 0);
  setPixel(72, 255, 0, 0);
  setPixel(73, 255, 0, 0);
  setPixel(74, 255, 0, 0);
  setPixel(75, 255, 0, 0);
  setPixel(76, 255, 0, 0);
  setPixel(77, 0, 255, 0);
  setPixel(78, 0, 255, 0);
  setPixel(79, 0, 255, 0);
  setPixel(80, 0, 255, 0);
  setPixel(81, 0, 255, 0);
  setPixel(82, 255, 0, 0);
  setPixel(83, 255, 0, 0);
  setPixel(84, 255, 0, 0);
  setPixel(85, 255, 0, 0);
  setPixel(86, 255, 0, 0);
  setPixel(87, 255, 0, 0);
  setPixel(88, 255, 0, 0);
  setPixel(89, 255, 0, 0);
  setPixel(90, 255, 0, 0);
  setPixel(91, 255, 0, 0);
  setPixel(92, 255, 0, 0);
  setPixel(93, 255, 0, 0);
  setPixel(94, 255, 0, 0);
  setPixel(95, 255, 0, 0);
  setPixel(96, 255, 0, 0);
  setPixel(97, 0, 255, 0);
  setPixel(98, 0, 255, 0);
  setPixel(99, 0, 255, 0);
  setPixel(100, 0, 255, 0);
  setPixel(101, 255, 0, 0);
  setPixel(102, 255, 0, 0);
  setPixel(103, 255, 0, 0);
  setPixel(104, 255, 0, 0);
  setPixel(105, 255, 0, 0);
  setPixel(106, 255, 0, 0);
  setPixel(107, 255, 0, 0);
  setPixel(108, 255, 0, 0);
  setPixel(109, 255, 0, 0);
  setPixel(110, 255, 0, 0);
  setPixel(111, 255, 0, 255);
  setPixel(112, 255, 0, 255);
  setPixel(113, 255, 0, 255);
  setPixel(114, 255, 0, 255);
  setPixel(115, 255, 0, 255);
  setPixel(116, 255, 0, 255);
  setPixel(117, 255, 0, 255);
  setPixel(118, 255, 0, 255);
  setPixel(119, 255, 0, 255);
  setPixel(120, 255, 0, 255);
  setPixel(121, 255, 0, 255);
  setPixel(122, 255, 0, 255);
  setPixel(123, 255, 0, 255);
  setPixel(124, 255, 0, 255);
  setPixel(125, 255, 0, 255);
  setPixel(126, 255, 0, 255);
  setPixel(127, 255, 0, 255);
  setPixel(128, 255, 0, 255);
  setPixel(129, 255, 0, 255);
  setPixel(130, 255, 0, 255);
  setPixel(131, 255, 0, 255);
  setPixel(132, 255, 0, 255);
  FastLED.show();
  FastLED.show();
}

void bigHeadEnding() {

  /*
    setPixel(111, 3, 0, 0);
    setPixel(112, 3, 0, 0);
    setPixel(113, 3, 0, 0);
    setPixel(114, 3, 0, 0);
    setPixel(115, 3, 0, 0);
    setPixel(116, 3, 0, 0);
    setPixel(117, 3, 0, 0);
    setPixel(118, 3, 0, 0);
    setPixel(119, 3, 0, 0);
    setPixel(120, 3, 0, 0);
    setPixel(121, 3, 0, 0);
    setPixel(122, 3, 0, 0);
    setPixel(123, 3, 0, 0);
    setPixel(124, 3, 0, 0);
    setPixel(125, 3, 0, 0);
    setPixel(126, 3, 0, 0);
    setPixel(127, 3, 0, 0);
  */
  setPixel(128, 10, 0, 0);
  setPixel(129, 10, 0, 0);
  setPixel(130, 10, 0, 0);
  setPixel(131, 10, 0, 0);
  setPixel(132, 10, 0, 0);
  FastLED.show();
  FastLED.show();
}

void pumpkinLights(int pumpkin) {

  int delayBetween = 1;

  switch (pumpkin) {
    case 1:
      for (int i = 0; i < 5; i++) {
        setPixel(i, 255, 0, 0);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(delayBetween);
      break;

    case 2:
      for (int i = 5; i < 10; i++) {
        setPixel(i, 255, 0, 0);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(delayBetween);
      break;

    case 3:
      for (int i = 10; i < 15; i++) {
        setPixel(i, 255, 0, 0);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(delayBetween);
      break;

    case 4:
      for (int i = 15; i < 17; i++) {               //Small Pumpkin
        setPixel(i, 255, 0, 0);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(200);
      break;

    case 5:
      for (int i = 17; i < 22; i++) {
        setPixel(i, 255, 0, 0);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(delayBetween);
      break;

    case 6:
      for (int i = 22; i < 27; i++) {
        setPixel(i, 255, 0, 0);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(delayBetween);
      break;

    case 7:
      for (int i = 27; i < 32; i++) {
        setPixel(i, 255, 0, 0);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(delayBetween);
      break;

    case 8:
      for (int i = 32; i < 37; i++) {
        setPixel(i, 255, 0, 0);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(delayBetween);
      break;

    case 9:
      for (int i = 37; i < 42; i++) {                //Colored 1
        setPixel(i, 0, 255, 0);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(delayBetween);
      break;

    case 10:
      for (int i = 42; i < 47; i++) {
        setPixel(i, 255, 0, 0);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(delayBetween);
      break;

    case 11:
      for (int i = 47; i < 52; i++) {
        setPixel(i, 255, 0, 0);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(delayBetween);
      break;

    case 12:
      for (int i = 52; i < 57; i++) {                 //Colored 2
        setPixel(i, 0, 255, 0);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(delayBetween);
      break;

    case 13:
      for (int i = 57; i < 62; i++) {
        setPixel(i, 255, 0, 0);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(delayBetween);
      break;

    case 14:
      for (int i = 62; i < 67; i++) {
        setPixel(i, 255, 0, 0);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(delayBetween);
      break;

    case 15:
      for (int i = 67; i < 72; i++) {
        setPixel(i, 255, 0, 0);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(delayBetween);
      break;

    case 16:
      for (int i = 72; i < 77; i++) {
        setPixel(i, 255, 0, 0);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(delayBetween);
      break;

    case 17:
      for (int i = 77; i < 82; i++) {               //Colored 3
        setPixel(i, 0, 255, 0);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(delayBetween);
      break;

    case 18:
      for (int i = 82; i < 87; i++) {
        setPixel(i, 255, 0, 0);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(delayBetween);
      break;

    case 19:
      for (int i = 87; i < 92; i++) {
        setPixel(i, 255, 0, 0);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(delayBetween);
      break;

    case 20:
      for (int i = 92; i < 97; i++) {
        setPixel(i, 255, 0, 0);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(delayBetween);
      break;

    case 21:
      for (int i = 97; i < 101; i++) {          //Colored 4 (4 Leds)
        setPixel(i, 0, 255, 0);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(delayBetween);
      break;

    case 22:
      for (int i = 101; i < 106; i++) {
        setPixel(i, 255, 0, 0);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(delayBetween);
      break;

    case 23:
      for (int i = 106; i < 111; i++) {
        setPixel(i, 255, 0, 0);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(delayBetween);
      break;


    case 24:
      for (int i = 111; i < 132; i++) {            // Chungus
        setPixel(i, 255, 0, 255);
      }
      delay(1);
      FastLED.show();
      FastLED.show();
      delay(delayBetween);
      break;
  }
}


void pumpkinBoot(int delayTime) {

  for (int i = 0; i < NUM_LEDS; i++) {                               //Colored Pumpkins
    if ( i == 37 || i == 38 || i == 39 || i == 40 || i == 41 ||
         i == 52 || i == 53 || i == 54 || i == 55 || i == 56 ||
         i == 77 || i == 78 || i == 79 || i == 80 || i == 81 ||
         i == 97 || i == 98 || i == 99 || i == 100) {
      setPixel(i, 0, 255, 0);
    }

    else if ( i >= 109 ) {                                           //BigHead Pumpkin
      setPixel(i, 150, 0, 255);
    }

    else {                                                           //Normal Pumpkins
      setPixel(i, 255, 0, 0);
    }

    delay(delayTime);
    FastLED.show();
  }
}

void forestSound() {
  digitalWrite(forestSoundRelay, HIGH);
  delay(200);
  digitalWrite(forestSoundRelay, LOW);
}

void laughingSound() {
  digitalWrite(laughingSoundRelay, HIGH);
  delay(200);
  digitalWrite(laughingSoundRelay, LOW);
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
  leds[Pixel].r = red;
  leds[Pixel].g = green;
  leds[Pixel].b = blue;
}


/** Maybe I'll figure out randomness one day...

  void prepareRandomArray() {

  randomSeed(millis());
  //chosenPumpkins[numberOfPumpkins] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

  for (int i = 0; i < numberOfPumpkins; i++) {
    int validPick = 1;
    int pumpkin = random(1, numberOfPumpkins + 1);

    Serial.print("Pumpkin: ");
    Serial.print(pumpkin);

    if (i == 0) {
      chosenPumpkins[i] = pumpkin;
    }

    else {
      for (int j = 0; j < numberOfPumpkins; j++) {
        if (pumpkin == chosenPumpkins[j]) {
          validPick = 0;
        }
        Serial.print("   i: ");
        Serial.print(i);
        Serial.print("   j: ");
        Serial.println(j);
      }

      if (validPick == 1) {
        chosenPumpkins[i] = pumpkin;
      }
      else {
        i--;
      }
    }
  }

  //Print
  Serial.print("Pumpkin Array: {");
  for (int i = 0; i < numberOfPumpkins; i++) {
    Serial.print(chosenPumpkins[i]);
    Serial.print(" ");
  }
  Serial.println("}");
  }

  void test() {

  pumpkinLights(24);
  }

**/


void wifiSetup() {

  //Serial
  Serial.begin(115200);
  delay(200);
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
  ArduinoOTA.setHostname("CreepyCorridor-Harvest");
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
