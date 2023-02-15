//IoT - Automação Residencial
//Temperatura, Umidade, Luminosidade
//Autor : Robson Brasil
//Versão : 13
//Última Modificação : 27/08/2020

// --- Bibliotecas Auxiliares ---
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>

char auth[] = "WW7eZUxja-yaJ04fANp6KF-z0N1K63Jg";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "RVR 2,4GHz"; // Enter Your WiFi Name
char pass[] = "RodrigoValRobson2021"; // Enter Your Passwword

// GPIO where the DS18B20 is connected to
const int oneWireBus = 0;   

#define DHTTYPE DHT22
#define DHTPIN 22 // Output Pin Of DHT  
int LDR = 36;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

void sendSensor()
{
  sensors.requestTemperatures(); 
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
  
  if (isnan(h) || isnan(t))
    {
    Serial.println("Failed to read Data from sensors!");
    return;
    }
    else
    {
      Serial.print(t);
      Serial.println("ºC");
      Serial.println("----------------------------------");
      Serial.print(h);
      Serial.println("----------------------------------");
      Serial.println("ºC");

    }
      Blynk.virtualWrite(V1, t); // Virtual Pin V5 for Temprature
      Blynk.virtualWrite(V2, h); // Virtual Pin V6 for Humidity
}
void setup() {

  pinMode(LDR,INPUT);

   // Start the Serial Monitor
  Serial.begin(115200);
  // Start the DS18B20 sensor
  dht.begin();
  sensors.begin();
  timer.setInterval(1000L, sendSensor);
  Blynk.begin(auth, ssid, pass);
}
  void loop()
{
  int val = analogRead(LDR); 
  Serial.println(val);
  Blynk.virtualWrite(V3,val);
  Blynk.run();
  timer.run();
 }
