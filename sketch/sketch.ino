#include "env.h"
#include "config.h"


// Módulo Motor.
// ---------------------------------
#include "MotorDriver.h"
void checkMotor() {
  motor.loopUpdate();
}
// ---------------------------------


// Módulo WiFi.
// ---------------------------------
#include "WifiConn.h"
WifiConn wifiConn(WIFI_MAX_ATTEMPTS, WIFI_CONNECTION_TIMEOUT);
// ---------------------------------


// Módulo Ventanas.
// ---------------------------------
bool ventanasEnUso() {
  return motor.estaEncendido();
}


void printVentanas() {
  Serial.print("Ventanas: ");
  if (ventanasEnUso()) {
    Serial.println(motor.getEstadoVentanas() ? "Abriendo..." : "Cerrando...");
  } else {
    Serial.println(motor.getEstadoVentanas() ? "Abiertas." : "Cerradas.");
  }
}
// ---------------------------------


// Módulo CO2
// ---------------------------------
#include "CO2Sensor.h"
CO2Sensor co2Sensor(CO2_SENSOR_PIN, CO2_SENSOR_DELAY);
float ultimoCO2;

void checkCO2() {
  ultimoCO2 = co2Sensor.getCO2();
  if (!co2Sensor.CO2Valida()) {
    Serial.println("Error al obtener el CO2.");
    ultimoCO2 = -1;
    return;
  }
}

void printCO2() {
  if (co2Sensor.CO2Valida()) {
    Serial.println("CO2: " + String(co2Sensor.getCO2()) + " ppm");
  }
}
// ---------------------------------


// Módulo MQTT.
// ---------------------------------
#include "MqttClient.h"
MqttClient mqttClient(wifiConn.getClienteInseguro());
const unsigned long intervaloPushMqtt = MQTT_INTERVALO_PUSH;
unsigned long ultimoPushMqtt;

void checkMqttConnection() {
  if (!mqttClient.isConnected()) {
    mqttClient.connect(MQTT_CLIENT_NAME, MQTT_CONN_DELAY);
  }
  mqttClient.loop();
}

static float ultimoCO2Enviado = -1000;

void publicarTopicosMqtt() {
  if ((millis() - ultimoPushMqtt) > intervaloPushMqtt) {
    if (ultimoCO2 != ultimoCO2Enviado) {
      char mensajeMqtt[MSG_MAX_LENGTH];
      snprintf(mensajeMqtt, MSG_MAX_LENGTH, "{\"CO2\":%.2f}", ultimoCO2);
      mqttClient.publish("esp32/co2", mensajeMqtt);
      ultimoCO2Enviado = ultimoCO2;
    }
    ultimoPushMqtt = millis();
  }
}
// ---------------------------------


void setup() {
  Serial.begin(115200);
  Serial.println("Configurando el ESP32...");
  motor.init(MOTOR_DEFAULT_ESTADO, MOTOR_DURACION_MS);
  co2Sensor.init();
  Serial.print("DEBUG conexión WiFi (esperado false): ");
  Serial.println(wifiConn.isConnected() ? "true" : "false");
  wifiConn.connect(WIFI_SSID, WIFI_PASSWORD);
  mqttClient.init(MQTT_BROKER_ADRESS, MQTT_PORT);
}

void loop() {
  Serial.println("Comienza loop...");
  checkMqttConnection();
  checkCO2();
  printCO2();
  printVentanas();
  checkMotor();
  publicarTopicosMqtt();
  delay(LOOP_DELAY);
}
