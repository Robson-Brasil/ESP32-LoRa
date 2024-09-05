// Bibliotecas
#include <WiFi.h>               // Função: Fornece suporte para comunicação Wi-Fi em projetos Arduino. Permite a conexão a redes Wi-Fi e a criação de servidores ou clientes.
#include <PubSubClient.h>       // Função: Biblioteca para implementação do protocolo MQTT em projetos Arduino. É comumente utilizada para comunicação entre dispositivos IoT.
#include <WiFiUdp.h>            // Função: Fornece funcionalidades relacionadas a comunicação UDP (User Datagram Protocol) sobre Wi-Fi.
#include <Arduino.h>            // Função: Cabeçalho padrão do Arduino, contém definições essenciais e macros necessárias para o desenvolvimento em Arduino.
#include <DNSServer.h>          // Função: Implementa um servidor DNS que pode ser usado em conjunto com a biblioteca ESPAsyncWebServer para criar servidores web.   https://github.com/zhouhan0126/DNSServer---esp32
#include <TimeLib.h>            // Função: Manipulação de tempo e data em projetos Arduino.
#include <freertos/FreeRTOS.h>  // Função: Inclui a biblioteca FreeRTOS, um sistema operacional de tempo real para microcontroladores.
#include <freertos/task.h>      // Função: Contém definições para tarefas do FreeRTOS.
#include <Wire.h>               // Função: Permite a comunicação I2C entre dispositivos.
#include <ESPmDNS.h>
#include <NetworkUdp.h>
#include <ArduinoOTA.h>