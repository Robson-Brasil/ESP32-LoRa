#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "IoT";
const char* password = "@IoT@S3nh@S3gur@";
const char* mqtt_server = "192.168.15.30";
#define MQTT_USER "RobsonBrasil"
#define MQTT_PASSWORD "loboalfa"

WiFiClient espClient;
PubSubClient client(espClient);

const int PIR = 23;
const int RELAY = 22;

void setup() {
  pinMode(PIR, INPUT);
  pinMode(RELAY, OUTPUT);
  Serial.begin(115200);
  while (!Serial);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  client.setServer(mqtt_server, 1883);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client",MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("connected");
    } else {
      Serial.print("failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void loop() {
  if (digitalRead(PIR) == HIGH) {
    Serial.println("Motion detected");
    client.publish("lora/motion", "1");
    digitalWrite(RELAY, HIGH);
    delay(5000);
    digitalWrite(RELAY, LOW);
  } else {
    Serial.println("No motion");
  }
  delay(500);
}
