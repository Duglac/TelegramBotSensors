//Standard Libraries
#include <stdio.h>
#include <WiFi.h>

//DS18B20 Libraries
#include <OneWire.h>
#include <DallasTemperature.h>
#define SENSOR_PIN  18
OneWire oneWire(SENSOR_PIN);
DallasTemperature DS18B20(&oneWire);

//TelegramBot Libraries
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <DHT.h>

//Max44009 Libraries
#include "Wire.h"
#include "Max44009.h"

Max44009 myLux(0x4A);
uint32_t lastDisplay = 0;
const int potPin = 33;
int potValue = 0;

#define BOTtoken "#BOTTOKEN#"  
const char* ssid = "#WIFINAME#";
const char* password = "#WIFIPASSWORD#";
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

String getDataCheck(){
  String message = "The temperature in the atrium is " + String(DS18B20.getTempCByIndex(0)) + " ºC \n";
  message += "The luminosity in the atrium is " + String(myLux.getLux()) + " lux \n";
  return message;
}


void setup() {
  Serial.begin(9600);
  //Connecting to Wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  int i = 1;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    String status = "Connecting to WiFi..";
    status += ". Attempt: ";
    status += String(i);
    Serial.println(status);
    i++;
    if(i >= 6){
      Serial.println("Couldn't connect to the Wi-Fi.");
      Serial.println("Entering Deep Sleep mode.");
      esp_sleep_enable_timer_wakeup(1000000*60*20);
      delay(1000);
      esp_deep_sleep_start();
    }
  }

  DS18B20.begin();
  Wire.begin();
  Wire.setClock(100000);
  DS18B20.requestTemperatures();
  bot.sendMessage("-1001981526843", getDataCheck(), "");
  if(analogRead(potPin) < 3000){
    bot.sendMessage("-1001981526843", "LOW BATTERY LEVEL! (" + String(analogRead(potPin)) + ")", "");
  }
  esp_sleep_enable_timer_wakeup(1000000*60*20);
  delay(1000);
  esp_deep_sleep_start();
}

void loop() {

}
