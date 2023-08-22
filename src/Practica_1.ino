
#include <UbiConstants.h>
#include <UbidotsEsp32Mqtt.h>
#include <UbiTypes.h>
#include <iostream>
#include <cmath>
#include <stdio.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "DHT.h"

#define DHTPIN 27
#define DHTTYPE DHT11
#define MI_ABS(x) ((x) < 0 ? -(x) : (x))

TFT_eSPI tft = TFT_eSPI();
DHT dht(DHTPIN, DHTTYPE);
/****************************************
 * Define Constants
 ****************************************/
const char *UBIDOTS_TOKEN = "BBFF-rva1GqRKd6wMpqEcSWFtE4mDQ2lH0z";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "UPBWiFi";      // Put here your Wi-Fi SSID
const char *WIFI_PASS = "";      // Put here your Wi-Fi password
const char *DEVICE_LABEL = "Practica1";   // Put here your Device label to which data  will be published
const char *TEMPERATURA_VARIABLE_LABEL = "temp"; // Temperatura
const char *HUMEDAD_VARIABLE_LABEL = "hum"; // humedad

const int PUBLISH_FREQUENCY = 5000; // Update rate in milliseconds

unsigned long timer;
uint8_t analogPin = 27; // Pin used to read data from GPIO34 ADC_CH6.


Ubidots ubidots(UBIDOTS_TOKEN);

/****************************************
 * Auxiliar Functions
 ****************************************/

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

/****************************************
 * Main Functions
 ****************************************/

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  // ubidots.setDebug(true);  // uncomment this to make debug messages available
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  Serial.println(F("DHTxx test!"));
  dht.begin();
  timer = millis();

  float hum = dht.readHumidity();
  float temp=dht.readTemperature();
  tft.init();
  tft.fillScreen(TFT_WHITE);
  tft.setRotation(3);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.drawString("Realizado por:", 10, 5, 2);
  tft.setTextColor(TFT_MAGENTA, TFT_WHITE);
  tft.drawString("Silvia Rueda", 10, 23, 4);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.drawString("Humedad", 10, 70, 2);
  tft.drawString("Temperatura", 140, 70, 2);
  tft.drawFastHLine(10, 50, 170, TFT_MAGENTA);
  tft.fillRect(110, 65, 3, 80, TFT_MAGENTA);
  
  if(isnan(hum) || isnan(temp)){
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
}

void loop(){
  // put your main code here, to run repeatedly:
  if (!ubidots.connected())
  {
    ubidots.reconnect();
  }
  float hum = dht.readHumidity();
  float temp=dht.readTemperature();
  
  if ( (MI_ABS(millis() - timer)) > PUBLISH_FREQUENCY) // triggers the routine every 5 seconds
  {
    Serial.print("Temperatura: ");
    Serial.print(temp);
    Serial.print(" | Humedad: ");
    Serial.println(hum);
    tft.drawString(String(temp), 150, 100);
    tft.drawString(String(hum), 20, 100);
    ubidots.add(TEMPERATURA_VARIABLE_LABEL, temp);  
    ubidots.add(HUMEDAD_VARIABLE_LABEL, hum);   
        ubidots.publish(DEVICE_LABEL); 

    timer = millis();
  }
  
  ubidots.loop();
}
