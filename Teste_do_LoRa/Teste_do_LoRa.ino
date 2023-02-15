#include "Arduino.h"
#include "heltec.h"
 
void setup () {
 Heltec.begin(true, false, true);
 
 Heltec.display->setContrast(255);
 Heltec.display->clear();
  
 Heltec.display->setFont(ArialMT_Plain_24);
 Heltec.display->drawString(0, 0, " LoRa WiFi");
  
 Heltec.display->setFont(ArialMT_Plain_16);
 Heltec.display->drawString(0, 25, "   Robson Brasil");
  
 Heltec.display->setFont(ArialMT_Plain_10);
 Heltec.display->drawString(0, 45, "www.robson.brasil.com.br");
 
 Heltec.display->display();
}
 
void loop () {}
