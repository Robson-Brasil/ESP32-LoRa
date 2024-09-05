/******************************************************************************************************************************************
  IoT - Automação Residencial
  Autor : Robson Brasil

  Dispositivos : ESP32 LoRa Heltec V.2
  Preferences--> URLs adicionais do Gerenciador de placas:
                                    ESP32  : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  Download Board ESP32 (x.x.x):
  Roteador AP
  Broker MQTT
  Versão : 1.0 - Release Candidate
  Última Modificação : 04/09/2024
******************************************************************************************************************************************/

#include <sMQTTBroker.h>
#include <WiFi.h>

const char* LoginMQTT = "RobsonBrasil"; // Altere aqui para o seu Login do MQTT Broker.
const char* SenhaMQTT = "S3nh@S3gur@";  // Altere aqui pra a susa senha do MQTT Broker.

class MyBroker : public sMQTTBroker
{
public:
    bool onEvent(sMQTTEvent *event) override
    {
        switch (event->Type())
        {
        case NewClient_sMQTTEventType:
        {
            sMQTTNewClientEvent *e = (sMQTTNewClientEvent *)event;
            // Verifique o nome de usuário e a senha usados para a nova conexão.
            if ((e->Login() != LoginMQTT) || (e->Password() != SenhaMQTT))
            {
                Serial.println("Login ou Senha inválido(s)");
                return false;
            }
        };
        break;
        case LostConnect_sMQTTEventType:
            WiFi.reconnect();
            break;
        case UnSubscribe_sMQTTEventType:
        case Subscribe_sMQTTEventType:
        {
            sMQTTSubUnSubClientEvent *e = (sMQTTSubUnSubClientEvent *)event;
        }
        break;
        }
        return true;
    }
};

MyBroker broker;

void setup()
{
    Serial.begin(115200);

    // Configurar IP estático no modo AP
    IPAddress local_IP(192,168,10,1);    // Define o IP que você deseja usar
    IPAddress gateway(192,168,10,1);     // Gateway, normalmente o mesmo do IP do AP
    IPAddress subnet(255,255,255,0);     // Máscara de sub-rede

    if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
        Serial.println("Falha na configuração do IP do AP!");
    }

    // Configurar Wi-Fi no modo Access Point
    WiFi.softAP("Roteador e MQTT Broker", "S3nh@S3gur@");  // Substitua pelo seu SSID e senha

    // Exibir o endereço IP do AP
    IPAddress IP = WiFi.softAPIP();
    Serial.print("Endereço IP do AP: ");
    Serial.println(IP);

    const unsigned short mqttPort = 1883;
    broker.init(mqttPort);

    // Código adicional, se necessário
}

void loop()
{
    broker.update();
    // Código adicional, se necessário
}