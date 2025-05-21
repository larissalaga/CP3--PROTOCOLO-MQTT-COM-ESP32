/////////--------IOT--------FIAP------------///////////

#include <Arduino.h>

#include <WiFi.h>
#include <ArduinoJson.h>
#include <DHTesp.h>
#include <PubSubClient.h>

// Identificadores
const char* ID        = "RM96496";
const char* moduleID  = "LarissaA-ESP32-Q4";

// Configurações de WiFi
const char *SSID = "Wokwi-GUEST";
const char *PASSWORD = "";  // Substitua pelo sua senha

// Configurações de MQTT
const char *BROKER_MQTT = "20.206.202.236";
const int BROKER_PORT = 1883;
const char *ID_MQTT = "RM96496";
const char* mqttPassword = "CP3-MQTT-rm96496";

// Tópico MQTT
#define TOPICO_PUBLISH  "RM96496/mqtt/q4"

WiFiClient espClient;
PubSubClient MQTT(espClient);

unsigned long publishUpdate = 0;
const int INTERVALO_ENVIO = 5000;
const int TAMANHO = 256;

void initWiFi() {
  Serial.print("Conectando ao Wi-Fi");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado com sucesso!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
}

void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.print("Conectando ao broker MQTT... ");
    if (MQTT.connect(ID_MQTT, ID_MQTT, mqttPassword)) {
      Serial.println("Conectado!");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(MQTT.state());
      Serial.println(" tentando novamente em 2 segundos...");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  initWiFi();
  initMQTT();
}

void loop() {
  if (!MQTT.connected()) {
    reconnectMQTT();
  }

  MQTT.loop();

  if (millis() - publishUpdate >= INTERVALO_ENVIO) {
    publishUpdate = millis();

    // Gerar dados simulados
    float temperatura = random(200, 350) / 10.0;
    float umidade = random(400, 800) / 10.0;
    float pressao = random(980, 1050);
    float altitude = random(0, 500);

    // Montar JSON
    StaticJsonDocument<TAMANHO> doc;
    doc["temperatura"] = temperatura;
    doc["umidade"] = umidade;
    doc["pressao"] = pressao;
    doc["altitude"] = altitude;

    char payload[TAMANHO];
    serializeJson(doc, payload);

    // Publicar no tópico
    MQTT.publish(TOPICO_PUBLISH, payload);
    Serial.print("Publicado: ");
    Serial.println(payload);
  }
}