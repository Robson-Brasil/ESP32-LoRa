//IoT - Automação Residencial
//Dispositivo : ESP32 LoRa V2
//Temperatura, Umidade, Luminosidade e OLED
//Autor : Robson Brasil
//Versão : 5
//Última Modificação : 20/07/2021

#include "DHTesp.h"
#include "heltec.h"
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
//#include <DallasTemperature.h>

char auth[] = "WW7eZUxja-yaJ04fANp6KF-z0N1K63Jg";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "RVR 2,4GHz"; // Enter Your WiFi Name
char pass[] = "RodrigoValRobson2021"; // Enter Your Passwword

BlynkTimer timer;

DHTesp dht;
int LDR = 37;
 
float currentTemp;
float currentHumidity;
float currentLuminosidade;
 
void displayReadingsOnOled() {
   
  String temperatureDisplay   = "Temperatura: "   + (String)currentTemp         +  "°C";
  String humidityDisplay      = "Humidade: "      + (String)currentHumidity     +   "%";
  String luminosidadeDisplay  = "Luminosidade: "  + (String)currentLuminosidade +  "lm";  
 
  // Clear the OLED screen
  Heltec.display->clear();
  // Prepare to display temperature
  Heltec.display->drawString(0, 0, temperatureDisplay);
  // Prepare to display humidity
  Heltec.display->drawString(0, 12, humidityDisplay);
  // Prepare to display luminosidade
  Heltec.display->drawString(0, 24, luminosidadeDisplay);
  // Display the readings
  Heltec.display->display();  
}
    
void setup()
{
  dht.setup(36, DHTesp::DHT11);
   
  currentTemp = dht.getTemperature();
  currentHumidity = dht.getHumidity();
  currentLuminosidade = analogRead(LDR);

  pinMode(LED,OUTPUT);
  digitalWrite(LED,HIGH);

  pinMode(LDR,INPUT);
  Blynk.begin(auth, ssid, pass);  
  
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Enable*/, false /*Serial Enable*/);
  displayReadingsOnOled();
}

void loop()
{
  float temperature = dht.getTemperature();
  float humidity = dht.getHumidity();
  float luminosidade = analogRead(LDR);

  Blynk.virtualWrite(V1, temperature); // Virtual Pin V5 for Temprature
  Blynk.virtualWrite(V2, humidity); // Virtual Pin V6 for Humidity
 
  if (temperature != currentTemp || humidity != currentHumidity || luminosidade != currentLuminosidade) {
    currentTemp = temperature;
    currentHumidity = humidity;
    currentLuminosidade = luminosidade;
    displayReadingsOnOled();

  float val = analogRead(LDR); 
  Serial.println(val);
  Blynk.virtualWrite(V3,val);
  Blynk.run();
  timer.run();   
  }
    
  delay(100 );
}