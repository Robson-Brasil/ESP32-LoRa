#ifndef OTAWEB_H
#define OTAWEB_H

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <Update.h>

class OTAWeb {
private:
    const char* ssid;
    const char* password;
    const uint32_t port;
    const uint32_t chipID;
    const String chipIDStr;
    const String version;
    const String infos;
    boolean otaAutorizado;
    WebServer server;

    String verifica;
    String serverIndex;
    String resultadoOk;
    String resultadoFalha;

    void setupServer() {
        server.on("/", HTTP_GET, [this]() {
            server.sendHeader("Connection", "close");
            server.send(200, "text/html", verifica);
        });

        server.on("/avalia", HTTP_POST, [this]() {
            Serial.println("Em server.on /avalia: args= " + String(server.arg("autorizacao"))); //somente para debug

            if (server.arg("autorizacao") != "S3nh@S3gur@") {
                server.sendHeader("Connection", "close");
                server.send(200, "text/html", resultadoFalha);
            } else {
                otaAutorizado = true;
                server.sendHeader("Connection", "close");
                server.send(200, "text/html", serverIndex);
            }
        });

        server.on("/serverIndex", HTTP_GET, [this]() {
            server.sendHeader("Connection", "close");
            server.send(200, "text/html", serverIndex);
        });

        server.on("/update", HTTP_POST, [this]() {
            if (!otaAutorizado) {
                server.sendHeader("Connection", "close");
                server.send(200, "text/html", resultadoFalha);
                return;
            }

            server.sendHeader("Connection", "close");
            server.send(200, "text/html", (Update.hasError()) ? resultadoFalha : resultadoOk);
            delay(1000);
            ESP.restart();
        }, [this]() {
            HTTPUpload& upload = server.upload();
            if (upload.status == UPLOAD_FILE_START) {
                Serial.setDebugOutput(true);
                Serial.printf("Atualizando: %s\n", upload.filename.c_str());
                if (!Update.begin()) {
                    Update.printError(Serial);
                }
            } else if (upload.status == UPLOAD_FILE_WRITE) {
                if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                    Update.printError(Serial);
                }
            } else if (upload.status == UPLOAD_FILE_END) {
                if (Update.end(true)) {
                    Serial.printf("Atualização bem sucedida! %u\nReiniciando...\n", upload.totalSize);
                } else {
                    Update.printError(Serial);
                }
                Serial.setDebugOutput(false);
            } else {
                Serial.printf("Atualização falhou inesperadamente! (possivelmente a conexão foi perdida.): status=%d\n", upload.status);
            }
        });

        server.begin();

        Serial.println(infos);
        Serial.print("Servidor em: ");
        Serial.println(WiFi.localIP().toString() + ":" + String(port));
    }

public:
    OTAWeb(const char* ssid, const char* password, uint32_t port = 8181)
    : ssid(ssid), password(password), port(port),
      chipID((uint32_t)(ESP.getEfuseMac() >> 32)),
      chipIDStr("<p> Chip ID: " + String(chipID) + "</p>"),
      version("<p> Versão: 3.0 </p>"),
      infos(version + chipIDStr),
      otaAutorizado(false),
      server(port),
      verifica("<!DOCTYPE html><html><head><title>ESP32 WebServerOTA</title><meta charset='UTF-8'></head><body><h1>ESP32 LoRa WebServerOTA</h1><h2>Digite a chave de verificação.<p>Clique em ok para continuar. . .</p></h2>" + infos + "<form method='POST' action='/avalia' enctype='multipart/form-data'> <p><label>Autorização: </label><input type='text' name='autorizacao'></p><input type='submit' value='Ok'></form></body></html>"),
      serverIndex("<!DOCTYPE html><html><head><title>ESP32 LoRa WebServerOTA</title><meta charset='UTF-8'></head><body><h1>ESP32 LoRa WebServerOTA</h1><h2>Selecione o arquivo para a atualização e clique em atualizar.</h2>" + infos + "<form method='POST' action='/update' enctype='multipart/form-data'><p><input type='file' name='update'></p><p><input type='submit' value='Atualizar'></p></form></body></html>"),
      resultadoOk("<!DOCTYPE html><html><head><title>ESP32 LoRa WebServerOTA</title><meta charset='UTF-8'></head><body><h1>ESP32 LoRa WebServerOTA</h1><h2>Atualização bem sucedida!</h2>" + infos + "</body></html>"),
      resultadoFalha("<!DOCTYPE html><html><head><title>ESP32 LoRa WebServerOTA</title><meta charset='UTF-8'></head><body><h1>ESP32 LoRa WebServerOTA</h1><h2>Falha durante a atualização. A versão anterior será recarregado.</h2>" + infos + "</body></html>")
    {}

    void begin() {
        Serial.begin(115200);
        WiFi.mode(WIFI_AP_STA);
        WiFi.begin(ssid, password);

        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }

        Serial.println("");
        Serial.print("Connected to ");
        Serial.println(ssid);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());

        if (WiFi.status() == WL_CONNECTED) {
            setupServer();
        } else {
            Serial.println("Falha ao conectar ao WiFi.");
        }
    }

    void handle() {
        server.handleClient();
        delay(1);
    }
};

#endif // OTAWEB_H
