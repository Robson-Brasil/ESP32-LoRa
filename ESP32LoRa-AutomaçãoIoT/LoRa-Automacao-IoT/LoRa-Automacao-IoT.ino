/**********************************************************************************
  IoT - Automação Residencial
  Autor : Robson Brasil
  
  Dispositivos : ESP32 LoRa V2 Heltec, Módulo Relé de 8 Canais
  Preferences--> URLs adicionais do Gerenciador de placas:
                                    ESP8266: http://arduino.esp8266.com/stable/package_esp8266com_index.json,
                                    ESP32  : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  Download Board ESP32 (x.x.x):
  OTA e OTA WebServer
  Broker MQTT
  Versão : 2 - Beta
  Última Modificação : 30/08/2024
**********************************************************************************/

//Bibliotecas
#include "LoginsSenhas.h"
#include "TopicosMQTT.h"
#include "Bibliotecas.h"
#include "GPIOs.h"
#include "MQTT.h"
#include "VariaveisGlobais.h"
#include "Array.h"

// Variável global
float diff = 1.0;

// Configuração do servidor DNS
DNSServer dns;

// Configuração de IP estático
IPAddress local_IP(192, 168, 43, 130);
IPAddress gateway(192, 168, 43, 1);
IPAddress subnet(255, 255, 255, 0);

// Configuração de DNS estático
IPAddress primaryDNS(1, 1, 1, 1);
IPAddress secondaryDNS(8, 8, 8, 8);

void setup1();  // Declaração da função setup1()
void loop1();   // Declaração da função loop1()

#define MSG_BUFFER_SIZE (1000)

unsigned long lastMsgMQTT = 0;
unsigned long delayTime = 0;
int value = 0;

// Protótipos
void initSerial();
void initWiFi();
void initMQTT();
void reconectWiFi();
void MQTT_CallBack(char* topic, byte* payload, unsigned int length);
void VerificaConexoesWiFIeMQTT();
void initOutput();
void initESPmDNS();
void initOTA();

void initOTA(){
    // Port defaults to 3232
   ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
   ArduinoOTA.setHostname("ESP32-LoRa");

  // No authentication by default
   ArduinoOTA.setPassword("S3nh@S3gur@");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else {  // U_SPIFFS
        type = "filesystem";
      }

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
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
}

// Função: Inicializa o output em nível lógico baixo
void initOutput() {
  for (int i = 0; i < numRelays; i++) {
    pinMode(RelayPins[i], OUTPUT);
    digitalWrite(RelayPins[i], HIGH);  // Durante a partida, todos os Relés iniciam desligados
  }

  // Adiciona um pequeno delay para estabilizar o estado inicial dos pinos
  delay(500);

  // Durante a partida o LED WiFI, inicia desligado
  pinMode(wifiLed, OUTPUT);
  digitalWrite(wifiLed, HIGH);
}

//Função: Inicializa comunicação serial com baudrate 115200 (para fins de monitorar no terminal serial
void initSerial() {
  Serial.begin(115200);
}

//Função: Inicializa e conecta-se na rede WI-FI desejada
void initWiFi() {

  delay(1000);

  Serial.println("------Conexao WI-FI------");
  Serial.print("Conectando-se na rede: ");
  Serial.println(ssid);
  Serial.println("Aguarde");

  reconectWiFi();
}

// Função: Inicializa parâmetros de conexão MQTT(endereço do broker, porta e seta função de callback)
void initMQTT() {

  MQTT.setServer(BrokerMQTT, PortaBroker);  // Informa qual broker e porta deve ser conectado
  MQTT.setCallback(MQTT_CallBack);          // Atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
  MQTT.setKeepAlive(MQTT_KeepAlive);        // Defina o keep-alive
}

//Implementação das Funções Principais do Core0 do ESP32
void setup() {
  // Inicializações:
  initOutput();
  initSerial();
  initWiFi();
  //initMQTT();
  initOTA();

  Serial.println();
  Serial.println();

  // Adiciona um pequeno delay para estabilizar o estado inicial dos pinos
  delay(500);

  //Chama a função setup1()
  setup1();

}

// Programa Principal do Core0 do ESP32
void loop() {
  // Garante funcionamento das conexões WiFi e ao Broker MQTT
  //VerificaConexoesWiFIeMQTT();
  //Keep-Alive da comunicação com Broker MQTT
  //MQTT.loop();

  ArduinoOTA.handle();

  loop1();

  delay(500);
}

// Função: Inicializa o callback, esta função é chamada toda vez que uma informação de um dos tópicos subescritos chega.
void MQTT_CallBack(char* topic, byte* payload, unsigned int length) {

  //Serial.print("Mensagem enviada ao Broker MQTT no Tópico -> [");
  //Serial.print(topic);
  //Serial.print("] ");
  payload[length] = '\0';
  String data = "";

  if (strstr(topic, sub0)) {
    for (unsigned int i = 0; i < length; i++) {
      //Serial.print((char)payload[i]);
      data += (char)payload[i];
    }

    if ((char)payload[0] == '0') {

      digitalWrite(RelayPin1, LOW);  // Ligua o relé. Note que HIGH é o nível de tensão.
      digitalWrite(RelayPin2, LOW);  // Ligua o relé. Note que HIGH é o nível de tensão.
      digitalWrite(RelayPin3, LOW);  // Ligua o relé. Note que HIGH é o nível de tensão.
      digitalWrite(RelayPin4, LOW);  // Ligua o relé. Note que HIGH é o nível de tensão.
      digitalWrite(RelayPin5, LOW);  // Ligua o relé. Note que HIGH é o nível de tensão.
      digitalWrite(RelayPin6, LOW);  // Ligua o relé. Note que HIGH é o nível de tensão.
      digitalWrite(RelayPin7, LOW);  // Ligua o relé. Note que HIGH é o nível de tensão.
      digitalWrite(RelayPin8, LOW);  // Ligua o relé. Note que HIGH é o nível de tensão.
      status_todos = 0;
      toggleState_0 = 0;
      MQTT.publish(pub0, "0");
    } else {
      digitalWrite(RelayPin1, HIGH);  // Desligua o Relé tornando a tensão BAIXA
      digitalWrite(RelayPin2, HIGH);  // Desligua o Relé tornando a tensão BAIXA
      digitalWrite(RelayPin3, HIGH);  // Desligua o Relé tornando a tensão BAIXA
      digitalWrite(RelayPin4, HIGH);  // Desligua o Relé tornando a tensão BAIXA
      digitalWrite(RelayPin5, HIGH);  // Desligua o Relé tornando a tensão BAIXA
      digitalWrite(RelayPin6, HIGH);  // Desligua o Relé tornando a tensão BAIXA
      digitalWrite(RelayPin7, HIGH);  // Desligua o Relé tornando a tensão BAIXA
      digitalWrite(RelayPin8, HIGH);  // Desligua o Relé tornando a tensão BAIXA
      status_todos = 1;
      toggleState_0 = 1;
      MQTT.publish(pub0, "1");
    }
  }
  if (strstr(topic, sub1)) {
    for (unsigned int i = 0; i < length; i++) {
      //Serial.print((char)payload[i]);
      data += (char)payload[i];
    }
    //Serial.println();

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin1, LOW);  // Ligua o relé. Note que HIGH é o nível de tensão.
      toggleState_1 = 0;
      MQTT.publish(pub1, "0", true);
    } else {
      digitalWrite(RelayPin1, HIGH);  // Desligua o Relé tornando a tensão BAIXA
      toggleState_1 = 1;
      MQTT.publish(pub1, "1", true);
    }
  }
  if (strstr(topic, sub2)) {
    for (unsigned int i = 0; i < length; i++) {
      //Serial.print((char)payload[i]);
      data += (char)payload[i];
    }
    //Serial.println();

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin2, LOW);  // Ligua o relé. Note que HIGH é o nível de tensão.
      toggleState_2 = 0;
      MQTT.publish(pub2, "0", true);
    } else {
      digitalWrite(RelayPin2, HIGH);  // Desligua o Relé tornando a tensão BAIXA
      toggleState_2 = 1;
      MQTT.publish(pub2, "1", true);
    }
  }
  if (strstr(topic, sub3)) {
    for (unsigned int i = 0; i < length; i++) {
      //Serial.print((char)payload[i]);
      data += (char)payload[i];
    }
    //Serial.println();

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin3, LOW);  // Ligua o relé. Note que HIGH é o nível de tensão.
      toggleState_3 = 0;
      MQTT.publish(pub3, "0", true);
    } else {
      digitalWrite(RelayPin3, HIGH);  // Desligua o Relé tornando a tensão BAIXA
      toggleState_3 = 1;
      MQTT.publish(pub3, "1", true);
    }
  }
  if (strstr(topic, sub4)) {
    for (unsigned int i = 0; i < length; i++) {
      //Serial.print((char)payload[i]);
      data += (char)payload[i];
    }
    //Serial.println();

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin4, LOW);  // Ligua o relé. Note que HIGH é o nível de tensão.
      toggleState_4 = 0;
      MQTT.publish(pub4, "0", true);
    } else {
      digitalWrite(RelayPin4, HIGH);  // Desligua o Relé tornando a tensão BAIXA
      toggleState_4 = 1;
      MQTT.publish(pub4, "1", true);
    }
  }
  if (strstr(topic, sub5)) {
    for (unsigned int i = 0; i < length; i++) {
      //Serial.print((char)payload[i]);
      data += (char)payload[i];
    }
    //Serial.println();

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin5, LOW);  // Ligua o relé. Note que HIGH é o nível de tensão.
      toggleState_5 = 0;
      MQTT.publish(pub5, "0", true);
    } else {
      digitalWrite(RelayPin5, HIGH);  // Desligua o Relé tornando a tensão BAIXA
      toggleState_5 = 1;
      MQTT.publish(pub5, "1", true);
    }
  }
  if (strstr(topic, sub6)) {
    for (unsigned int i = 0; i < length; i++) {
      //Serial.print((char)payload[i]);
      data += (char)payload[i];
    }
    //Serial.println();

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin6, LOW);  // Ligua o relé. Note que HIGH é o nível de tensão.
      toggleState_6 = 0;
      MQTT.publish(pub6, "0", true);
    } else {
      digitalWrite(RelayPin6, HIGH);  // Desligua o Relé tornando a tensão BAIXA
      toggleState_6 = 1;
      MQTT.publish(pub6, "1", true);
    }
  }
  if (strstr(topic, sub7)) {
    for (unsigned int i = 0; i < length; i++) {
      //Serial.print((char)payload[i]);
      data += (char)payload[i];
    }
    //Serial.println();

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin7, LOW);  // Ligua o relé. Note que HIGH é o nível de tensão.
      toggleState_7 = 0;
      MQTT.publish(pub7, "0", true);
    } else {
      digitalWrite(RelayPin7, HIGH);  // Desligua o Relé tornando a tensão BAIXA
      toggleState_7 = 1;
      MQTT.publish(pub7, "1", true);
    }
  }
  if (strstr(topic, sub8)) {
    for (unsigned int i = 0; i < length; i++) {
      //Serial.print((char)payload[i]);
      data += (char)payload[i];
    }
    //Serial.println();

    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin8, LOW);  // Ligua o relé. Note que HIGH é o nível de tensão.
      toggleState_8 = 0;
      MQTT.publish(pub8, "0", true);
    } else {
      digitalWrite(RelayPin8, HIGH);  // Desligua o Relé tornando a tensão BAIXA
      toggleState_8 = 1;
      MQTT.publish(pub8, "1", true);
    }
  }
  //Serial.println("");
}

/* Função: reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
   em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito.*/
void reconnectMQTT() {

  unsigned long currentTime = millis();
  unsigned long reconnectTime = 2000;  // Tempo para tentar reconectar (em milissegundos)

  // Verifica o overflow de millis
  if (currentTime < delayTime) {
    /* Overflow ocorreu
       Lógica para lidar com o overflow, se necessário
       Por exemplo, reiniciar o último tempo para o valor atual */
    delayTime = currentTime;
  } else {
    delayTime = currentTime;

    while (!MQTT.connected()) {
      Serial.print(".....Tentando se conectar ao Broker MQTT: ");
      Serial.println(BrokerMQTT);
      if (MQTT.connect(ID_MQTT, LoginDoMQTT, SenhaMQTT)) {
        Serial.println("Conectado com sucesso ao broker MQTT!");
        Serial.println("");
        MQTT.subscribe(sub0);
        MQTT.subscribe(sub1);
        MQTT.subscribe(sub2);
        MQTT.subscribe(sub3);
        MQTT.subscribe(sub4);
        MQTT.subscribe(sub5);
        MQTT.subscribe(sub6);
        MQTT.subscribe(sub7);
        MQTT.subscribe(sub8);
        MQTT.subscribe(sub9);
      } else {
        Serial.println("Falha ao reconectar no broker.");
        Serial.print(MQTT.state());
        // Verifica o overflow de millis para o próximo tempo de reconexão
        if (millis() - currentTime > reconnectTime) {
          Serial.println("Haverá nova tentativa de conexão em 2s");
          // Reinicia o último tempo para o valor atual
          currentTime = millis();
        }
      }
    }
  }
}

// Função: Reconectar-se ao WiFi
void reconectWiFi() {
  /* Se já está conectado a rede WI-FI, nada é feito.
    Caso contrário, são efetuadas tentativas de conexão */
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(ssid, password);  // Conecta na rede WI-FI
  Serial.println("\nConectando WiFi " + String(ssid));

  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Conexão Falhou");
  }

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Conectado com sucesso na rede ");
  Serial.print(ssid);
  Serial.println("");
  Serial.println("WiFi Conectado");
  Serial.print("Endereço de IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS 1: ");
  Serial.println(WiFi.dnsIP(0));
  Serial.print("DNS 2: ");
  Serial.println(WiFi.dnsIP(1));
  Serial.println("");
}

/* Função: Verifica o estado das conexões WiFI e ao broker MQTT.
   Em caso de desconexão (qualquer uma das duas), a conexão  é refeita. */
void VerificaConexoesWiFIeMQTT() {

  if (!MQTT.connected())

    reconnectMQTT();  // se não há conexão com o Broker, a conexão é refeita

  reconectWiFi();  // se não há conexão com o WiFI, a conexão é refeita "apagar essa linha depois pra testar"
}

//Implementação das Funções Principais do Core1 do ESP32
void setup1() {

  // Adiciona um pequeno delay para estabilizar o estado inicial dos pinos
  delay(500);

  initMQTT();
}

void loop1() {
  // Garante funcionamento das conexões WiFi e ao Broker MQTT
  VerificaConexoesWiFIeMQTT();
  // Keep-Alive da comunicação com Broker MQTT
  MQTT.loop();  // Verifica se há novas mensagens no Broker MQTT

  unsigned long currentTimeMQTT = millis();

  // Verifica o overflow de millis
  if (currentTimeMQTT < lastMsgMQTT) {
    /* Overflow ocorreu
       Lógica para lidar com o overflow, se necessário
       Por exemplo, reiniciar o último tempo para o valor atual */
    lastMsgMQTT = currentTimeMQTT;
  } else {
    if (currentTimeMQTT - lastMsgMQTT > 100) {
      lastMsgMQTT = currentTimeMQTT;

      // Código executado a cada 100 milissegundos

      if (digitalRead(RelayPin1) == LOW) {
        MQTT.publish(pub1, "0", true);
      } else {
        MQTT.publish(pub1, "1", true);
      }
      if (digitalRead(RelayPin2) == LOW) {
        MQTT.publish(pub2, "0", true);
      } else {
        MQTT.publish(pub2, "1", true);
      }
      if (digitalRead(RelayPin3) == LOW) {
        MQTT.publish(pub3, "0", true);
      } else {
        MQTT.publish(pub3, "1", true);
      }
      if (digitalRead(RelayPin4) == LOW) {
        MQTT.publish(pub4, "0", true);
      } else {
        MQTT.publish(pub4, "1", true);
      }
      if (digitalRead(RelayPin5) == LOW) {
        MQTT.publish(pub5, "0", true);
      } else {
        MQTT.publish(pub5, "1", true);
      }
      if (digitalRead(RelayPin6) == LOW) {
        MQTT.publish(pub6, "0", true);
      } else {
        MQTT.publish(pub6, "1", true);
      }
      if (digitalRead(RelayPin7) == LOW) {
        MQTT.publish(pub7, "0", true);
      } else {
        MQTT.publish(pub7, "1", true);
      }
      if (digitalRead(RelayPin8) == LOW) {
        MQTT.publish(pub8, "0", true);
      } else {
        MQTT.publish(pub8, "1", true);
      }
      if (status_todos == 1) {
        MQTT.publish(pub0, "0", true);
      } else {
        MQTT.publish(pub0, "1", true);
      }
      if (status_desligatodos == 0) {
        MQTT.publish(pub9, "1", true);
      }
    }
  }
}
