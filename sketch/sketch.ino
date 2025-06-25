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


// Módulo Temperatura.
// ---------------------------------
#include "TempSensor.h"
TempSensor tempSensor(TEMP_SENSOR_PIN, TEMP_SENSOR_DELAY);
float ultimaTemperatura;

void checkTemperatura() {
  ultimaTemperatura = tempSensor.getTemperatura();
  if (!tempSensor.temperaturaValida()) {
    Serial.println("Error al obtener la temperatura.");
    ultimaTemperatura = -1;
    return;
  }
}

void printTemperatura() {
  if (tempSensor.temperaturaValida()) {
    Serial.println("Temperatura: " + String(tempSensor.getTemperatura()) + " °C.");
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

static float ultimaTempEnviada = -1000;

void publicarTopicosMqtt() {
  if ((millis() - ultimoPushMqtt) > intervaloPushMqtt) {
    if (ultimaTemperatura != ultimaTempEnviada) {
      char mensajeMqtt[MSG_MAX_LENGTH];
      snprintf(mensajeMqtt, MSG_MAX_LENGTH, "{\"temperatura\":%.2f}", ultimaTemperatura);
      mqttClient.publish("esp32/sensores", mensajeMqtt);
      ultimaTempEnviada = ultimaTemperatura;
    }
    ultimoPushMqtt = millis();
  }
}
// ---------------------------------


void setup() {
  Serial.begin(115200);
  Serial.println("Configurando el ESP32...");
  motor.init(MOTOR_DEFAULT_ESTADO, MOTOR_DURACION_MS);
  tempSensor.init();
  Serial.print("DEBUG conexión WiFi (esperado false): ");
  Serial.println(wifiConn.isConnected() ? "true" : "false");
  wifiConn.connect(WIFI_SSID, WIFI_PASSWORD);
  mqttClient.init(MQTT_BROKER_ADRESS, MQTT_PORT);
}

void loop() {
  Serial.println("Comienza loop...");
  checkMqttConnection();
  checkTemperatura();
  printTemperatura();
  printVentanas();
  checkMotor();
  publicarTopicosMqtt();
  delay(LOOP_DELAY);
}
