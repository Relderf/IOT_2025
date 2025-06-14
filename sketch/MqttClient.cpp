#include "MqttClient.h"
#include "MotorDriver.h"
#include "config.h"

MqttClient::MqttClient(WiFiClient& clienteWifi)
        : pubSubClient(clienteWifi) {
}

void MqttClient::init(const char* mqttAddress, uint16_t mqttPort) {
    pubSubClient.setServer(mqttAddress, mqttPort);
}

bool MqttClient::isConnected() {
    return pubSubClient.connected();
}

void MqttClient::connect(const char* clientName, int connDelay) {
    while (!pubSubClient.connected()) {
        Serial.print("MQTT connection...");
        if (pubSubClient.connect(clientName)) {
            Serial.println(" connected");
            pubSubClient.subscribe("esp32/ventanas");
        } else {
            Serial.print("failed, rc=");
            Serial.print(pubSubClient.state());
            Serial.println(" try again in " + String(connDelay) + "ms.");
            delay(connDelay);
        }
    }
}

void MqttClient::loop() {
    pubSubClient.loop();
}

void MqttClient::publish(const char* topic, const char* payload) {
    pubSubClient.publish(topic, payload);
}

void MqttClient::callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String comando((char*)payload, length);
    
    if (String(topic) == "esp32/ventanas") {
        if (comando == "abrir") {
            motor.abrirVentanas();
        } else if (comando == "cerrar") {
            motor.cerrarVentanas();
        }
    }
}