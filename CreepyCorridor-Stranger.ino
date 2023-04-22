



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

//MP3 Player
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>
DFRobotDFPlayerMini myDFPlayer;
SoftwareSerial mySoftwareSerial(D4, D2);  //Pins for MP3 Player Serial (RX, TX)

//Initialize and Subscribe to MQTT
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish creepyPublish = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/creepy-corridor.creepymode");
Adafruit_MQTT_Subscribe creepyMode = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/creepy-corridor.creepymode");

//Variables
int operationalMode = 1;
int lastOperationalMode = 0;
int showTime = 0;
char songSelection = 1;
uint16_t simulateTrigger = 0;
float mqttConnectFlag = 0.0;

//Input/Ouput
#define sensor D6




// ***************************************
// *************** Setup *****************
// ***************************************


void setup() {

  //Initialize Serial, WiFi, & OTA
  wifiSetup();

  //Initialize MQTT
  mqtt.subscribe(&creepyMode);
  MQTT_connect();

  //MP3
  mySoftwareSerial.begin(9600);
  delay(1000);
  myDFPlayer.begin(mySoftwareSerial);
  Serial.println();
  Serial.println("DFPlayer initialized!");

  myDFPlayer.setTimeOut(500); //Timeout serial 500ms
  myDFPlayer.volume(0); //Volume 0-30
  myDFPlayer.EQ(0); //Equalization normal
  delay(1000);
  myDFPlayer.volume(25);
  myDFPlayer.play(songSelection);
  delay(3000);
  myDFPlayer.stop();

  //All set
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
    else if (valueOperational == 101) {
      simulateTrigger = 1;
    }
    else {}
    delay(1);
  }

  //State Manager
  if (operationalMode == 2) {
    songSelection = 2;
    if(lastOperationalMode == 1){
      myDFPlayer.play(songSelection);
    }
    // Proper Time delay for song 158000ms.
  }
  
  else if (operationalMode == 1) {

    if(lastOperationalMode == 0){
      myDFPlayer.stop();
    }

    //TODO
    // 1. Sense peeps
    // 2. Send mqtt to Foggy for fog
    // 3. play Kids

   int sensorReading = digitalRead(sensor);
    if (sensorReading == HIGH || simulateTrigger == 1) {
      delay(100); //Prevents false readings
      int sensorReading2 = digitalRead(sensor);
      if (sensorReading2 == HIGH || simulateTrigger == 1) {
        delay(100);
        int sensorReading3 = digitalRead(sensor);
        if (sensorReading3 == HIGH || simulateTrigger == 1) {
          Serial.println("[Begin Stranger Performance]");
          Serial.println(sensorReading3);
          //**PLAY STRANGER
          myDFPlayer.play(songSelection);

          //**SEND FOG MACHINE MQTT
          delay(100);
          creepyPublish.publish(11);

          delay(35000);
          myDFPlayer.stop();
          simulateTrigger = 0;
          Serial.println("[End Stranger Performance]");
        }
      }
    }
  }

  lastOperationalMode = operationalMode;
  delay(1);
}



// ***************************************
// ********** Backbone Methods ***********
// ***************************************


void wifiSetup() {

  //Serial
  Serial.begin(115200);
  Serial.println("Booting");

  //WiFi and OTA
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  ArduinoOTA.setHostname("CreepyCorridor-Stranger");
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
