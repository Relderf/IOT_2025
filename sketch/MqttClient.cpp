#include "MqttClient.h"

MqttClient::MqttClient(WiFiClient& clienteWifi)
        : pubSubClient(clienteWifi) {
}

void callback(const char* topic, byte* message, unsigned int length) {
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String messageTemp;
    
    for (int i = 0; i < length; i++) {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
    Serial.println();
  
    // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
    // Changes the output state according to the message
    if (String(topic) == "esp32/output") {
        Serial.print("Changing output to ");
        if (messageTemp == "on") {
            Serial.println("on");
            // TODO: abrir ventanas???
        } else if (messageTemp == "off") {
            Serial.println("off");
            // TODO: cerrar ventanas???
        }
    }
}

void MqttClient::init(const char* mqttAddress, uint16_t mqttPort) {
    pubSubClient.setServer(mqttAddress, mqttPort);
    pubSubClient.setCallback(callback);
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