#include "MqttClient.h"
#include "CO2Sensor.h"
#include "config.h"

extern CO2Sensor co2Sensor;

static float ultimoCO2Enviado = -1000;
static unsigned long ultimoPushMqtt = 0;

MqttClient::MqttClient(WiFiClient& clienteWifi)
        : pubSubClient(clienteWifi) {
}

void MqttClient::init(const char* mqttAddress, uint16_t mqttPort) {
    pubSubClient.setServer(mqttAddress, mqttPort);
    pubSubClient.setCallback([this](char* topic, byte* payload, unsigned int length) {
        this->callback(topic, payload, length);
    });
}

bool MqttClient::isConnected() {
    return pubSubClient.connected();
}

void MqttClient::connect(const char* clientName, int connDelay) {
    while (!pubSubClient.connected()) {
        Serial.print("MQTT connection...");
        if (pubSubClient.connect(clientName)) {
            Serial.println(" connected");
            pubSubClient.subscribe(("camara/" + String(CAMARA_ID) + "/ventilacion").c_str());
            pubSubClient.subscribe(("camara/" + String(CAMARA_ID) + "/kilos").c_str());
            pubSubClient.subscribe(("camara/" + String(CAMARA_ID) + "/m3").c_str());
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
    char mensaje[length + 1];
    memcpy(mensaje, payload, length);
    mensaje[length] = '\0';  // null-terminador

    String comando = String(mensaje);

    if (String(topic) == "camara/" + String(CAMARA_ID) + "/ventilacion") {
        if (comando == "prender") {
            co2Sensor.setVentilacion(true);
            publicarEstadoVentilacion();
        } else if (comando == "apagar") {
            co2Sensor.setVentilacion(false);
            publicarEstadoVentilacion();
        }
    } else if (String(topic) == "camara/" + String(CAMARA_ID) + "/kilos") {
        co2Sensor.setPapasNormales(comando.toFloat());
    } else if (String(topic) == "camara/" + String(CAMARA_ID) + "/m3") {
        co2Sensor.setM3(comando.toFloat());
    }
}

void MqttClient::publicarCo2(float co2) {
    if ((millis() - ultimoPushMqtt) > MQTT_INTERVALO_PUSH) {
        if (co2 != ultimoCO2Enviado) {
            char mensajeMqtt[MSG_MAX_LENGTH];
            snprintf(mensajeMqtt, MSG_MAX_LENGTH, "{\"co2\":%.2f}", co2);
            pubSubClient.publish(("camara/" + String(CAMARA_ID) + "/co2").c_str(), mensajeMqtt);
            ultimoCO2Enviado = co2;
        }
        ultimoPushMqtt = millis();
    }
}

void MqttClient::publicarEstadoVentilacion() {
    char mensajeMqtt[MSG_MAX_LENGTH];
    snprintf(mensajeMqtt, MSG_MAX_LENGTH, "{\"ventilacion\":%s}", co2Sensor.getVentilacion() ? "true" : "false");
    pubSubClient.publish(("camara/" + String(CAMARA_ID) + "/estado_ventilacion").c_str(), mensajeMqtt);
}
