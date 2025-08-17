#include <WiFi.h>
#include <sMQTTBroker.h>

// Configurações do ponto de acesso
const char* ssid = "Roteador";
const char* password = "S3nh@S3gur@";

// Configurações de rede
IPAddress local_IP(192,168,10,1);
IPAddress gateway(192,168,10,1);
IPAddress subnet(255,255,255,0);

// Classe do broker MQTT
class MyBroker:public sMQTTBroker
{
public:
    bool onEvent(sMQTTEvent *event) override
    {
        switch(event->Type())
        {
        case NewClient_sMQTTEventType:
            {
                sMQTTNewClientEvent *e=(sMQTTNewClientEvent*)event;
                e->Login();
                e->Password();
            }
            break;
        case LostConnect_sMQTTEventType:
            WiFi.reconnect();
            break;
        case UnSubscribe_sMQTTEventType:
        case Subscribe_sMQTTEventType:
            {
                sMQTTSubUnSubClientEvent *e=(sMQTTSubUnSubClientEvent*)event;
            }
            break;
        }
        return true;
    }
};

MyBroker broker;
unsigned long Time;
unsigned long freeRam;

void setup() {
    Serial.begin(115200);

    // Configura o IP estático para o ponto de acesso
    if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
        Serial.println("Falha ao configurar o IP estático!");
    }

    // Inicializa o ponto de acesso
    WiFi.softAP(ssid, password);

    Serial.println("Ponto de acesso iniciado.");
    Serial.print("IP do ponto de acesso: ");
    Serial.println(WiFi.softAPIP());

    // Inicializa o broker MQTT na porta 1883
    const unsigned short mqttPort = 1883;
    broker.init(mqttPort);
    Serial.println("Broker MQTT iniciado.");

    // Configurações iniciais do loop
    Time = millis();
    freeRam = ESP.getFreeHeap();
}

void loop() {
    broker.update(); // Processa eventos do broker MQTT

    // Verifica e imprime a RAM livre a cada segundo
    if (millis() - Time > 1000) {
        Time = millis();
        if (ESP.getFreeHeap() != freeRam) {
            freeRam = ESP.getFreeHeap();
            Serial.print("RAM Livre:");
            Serial.println(freeRam);
        }
    }
}