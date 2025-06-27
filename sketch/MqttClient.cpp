#include "MqttClient.h"
#include "MotorDriver.h"
#include "CO2Sensor.h"
#include "config.h"

extern bool ventilacionActiva;
extern CO2Sensor co2Sensor;

static float ultimoCO2Enviado = -1000;
static unsigned long ultimoPushMqtt = 0;

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
            pubSubClient.subscribe("camara/01/ventilacion");
            pubSubClient.subscribe("camara/01/kilos");
            pubSubClient.subscribe("camara/01/m3");
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
    String comando((char*)payload, length);
    if (String(topic) == "camara/01/ventilacion") {
        if (comando == "prender") {
            ventilacionActiva = true;
            co2Sensor.setVentilacion(true);
            motor.encender();
        } else if (comando == "apagar") {
            ventilacionActiva = false;
            co2Sensor.setVentilacion(false);
            motor.apagar();
        }
    } else if (String(topic) == "camara/01/kilos") {
        float kilos = comando.toFloat();
        if (kilos > 0) {
            Serial.println("Kilos de papas actualizados: " + String(kilos));
        } else {
            Serial.println("Valor inválido para kilos: " + comando);
        }
    } else if (String(topic) == "camara/01/m3") {
        float m3 = comando.toFloat();
        if (m3 > 0) {
            Serial.println("Espacio en m3 actualizada: " + String(m3));
        } else {
            Serial.println("Valor inválido para m3: " + comando);
        }
    }
}

void MqttClient::publicarMqtt(float co2) {
    if ((millis() - ultimoPushMqtt) > MQTT_INTERVALO_PUSH) {
        if (co2 != ultimoCO2Enviado) {
            char mensajeMqtt[MSG_MAX_LENGTH];
            snprintf(mensajeMqtt, MSG_MAX_LENGTH, "{\"CO2\":%.2f}", co2);
            pubSubClient.publish("camara/01/co2", mensajeMqtt);
            ultimoCO2Enviado = co2;
        }
        ultimoPushMqtt = millis();
    }
}