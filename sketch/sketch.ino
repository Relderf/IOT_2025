#include "env.h"
#include "config.h"

// Módulo WiFi.
// ---------------------------------
#include "WifiConn.h"
WifiConn wifiConn(WIFI_MAX_ATTEMPTS, WIFI_CONNECTION_TIMEOUT);
// ---------------------------------


// Módulo CO2
// ---------------------------------
#include "CO2Sensor.h"
CO2Sensor co2Sensor(CO2_SENSOR_PIN, CO2_SENSOR_DELAY);

void printCO2() {
  if (co2Sensor.CO2Valida()) {
    Serial.println("CO2 actual: " + String(co2Sensor.getCO2()) + " ppm");
  } else {
    Serial.println("CO2 no válido o sensor no disponible.");
  }
}
// ---------------------------------


// Módulo Ventilación
// ---------------------------------
void printVentilacion() {
  Serial.print("Ventilación: ");
  Serial.println(co2Sensor.getVentilacion() ? "Prendida." : "Apagada.");
}
// ---------------------------------


// Módulo MQTT.
// ---------------------------------
#include "MqttClient.h"
MqttClient mqttClient(wifiConn.getClienteInseguro());
const unsigned long intervaloPushMqtt = MQTT_INTERVALO_PUSH;
static float ultimoCO2Enviado = -1000;
unsigned long ultimoPushMqtt;

void checkMqttConnection() {
  if (!mqttClient.isConnected()) {
    mqttClient.connect(MQTT_CLIENT_NAME, MQTT_CONN_DELAY);
  }
  mqttClient.loop();
}

void publicarCO2Mqtt() {
  mqttClient.publicarCo2(co2Sensor.getCO2());
}
// ---------------------------------


void setup() {
  Serial.begin(115200);
  Serial.println("Configurando el ESP32...");
  co2Sensor.init();
  Serial.print("DEBUG conexión WiFi (esperado false): ");
  Serial.println(wifiConn.isConnected() ? "true" : "false");
  wifiConn.connect(WIFI_SSID, WIFI_PASSWORD);
  mqttClient.init(MQTT_BROKER_ADRESS, MQTT_PORT);
}

void loop() {
  Serial.println("Comienza loop...");
  checkMqttConnection();
  printCO2();
  printVentilacion();
  publicarCO2Mqtt();
  delay(LOOP_DELAY);
}
