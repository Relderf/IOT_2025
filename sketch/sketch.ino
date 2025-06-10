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


// Def. Bot Telegram.
// ---------------------------------
#include "TelegramBot.h"
TelegramBot telegramBot(TOKEN_BOT, wifiConn.getCliente(), BOT_INTERVALO_CHEQUEO_MENSAJES,
                        [&wifiConn]() { return wifiConn.isConnected(); });
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


// Módulo Humedad.
// ---------------------------------
float randomHumedad() {
    return random(HUMEDAD_MIN_SIMULADA, HUMEDAD_MAX_SIMULADA + 1);
}

float ultimaHumedad = 0;

float simularHumedad() {
    static float humedadSimulada = 0;
    static float objetivo = 0;
    static unsigned long ultimaLecturaMs = 0;

    if ((millis() - ultimaLecturaMs) > 5000) {
        ultimaLecturaMs = millis();

        if (humedadSimulada == 0) {
            humedadSimulada = randomHumedad();
            objetivo = randomHumedad();
        }

        if (abs(humedadSimulada - objetivo) < 0.5) {
            objetivo = randomHumedad();
        } else {
            float paso = random(0, 21) / 10.0; // Paso de entre 0 y 2

            if (humedadSimulada < objetivo) {
                humedadSimulada += paso;
                if (humedadSimulada > objetivo) humedadSimulada = objetivo;
            } else {
                humedadSimulada -= paso;
                if (humedadSimulada < objetivo) humedadSimulada = objetivo;
            }
        }
    }
    return humedadSimulada;
}

void checkHumedad() {
  ultimaHumedad = simularHumedad();
}

void printHumedad() {
  Serial.println("Humedad: " + String(ultimaHumedad) + "%.");
}
// ---------------------------------


// Módulo Bot Telegram.
// ---------------------------------
const unsigned long intervaloNotificacioBot = BOT_INTERVALO_NOTIFICACION;
unsigned long ultimaNotificacionBot;

void comandoAyuda(String chatId) {
  Serial.println("TelegramBot: Informando ayuda...");
  String welcome = "¡Hola! Estos son los comandos disponibles:\n";
  welcome += "/activar - Activa el modo automático de ventanas.\n";
  welcome += "/desactivar - Desactiva el modo automático de ventanas.\n";
  welcome += "/abrir - Abre las ventanas.\n";
  welcome += "/cerrar - Cierra las ventanas.\n";
  welcome += "/estado - Informa el estado actual.\n";
  telegramBot.sendMessage(chatId, welcome);
}

void abrirVentanas(String chatId = "") {
  if (motor.puedeAbrirVentanas()) {
    Serial.println("Ventanas: abriendo...");
    motor.abrirVentanas();
    if (chatId != "") {
      telegramBot.sendMessage(chatId, "Abriendo ventanas...");
    }
  }
}

void cerrarVentanas(String chatId = "") {
  if (motor.puedeAbrirVentanas()) {
    Serial.println("Ventanas: cerrando...");
    motor.cerrarVentanas();
    if (chatId != "") {
      telegramBot.sendMessage(chatId, "Cerrando ventanas...");
    }
  } else {
    telegramBot.sendMessage(chatId, "Ventanas ya cerradas o en uso.");
  }
}

void comandoAbrirVentana(String chatId) {
  Serial.println("TelegramBot: Abriendo ventana...");
  abrirVentanas(chatId);
}

void comandoCerrarVentana(String chatId) {
  Serial.println("TelegramBot: Cerrando ventana...");
  cerrarVentanas(chatId);
}

void comandoInformarEstado(String chatId) {
  Serial.println("TelegramBot: Informando estado...");
  String estado = "Temperatura: " + String(ultimaTemperatura) + " °C.\n";
  String estadoVentanasMsj;
  if (ventanasEnUso()) {
    estadoVentanasMsj = (motor.getEstadoVentanas() ? "Abriendo..." : "Cerrando...");
  } else {
    estadoVentanasMsj = (motor.getEstadoVentanas() ? "Abiertas." : "Cerradas.");
  }
  estado += ("Ventanas: " + estadoVentanasMsj);
  telegramBot.sendMessage(chatId, estado);
}

void checkTelegramBot() {
  telegramBot.checkMensajes();
  if ((millis() - ultimaNotificacionBot) > intervaloNotificacioBot) {
    comandoInformarEstado(GROUP_CHAT_ID);
    ultimaNotificacionBot = millis();
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
static float ultimaHumEnviada = -1000;

void publicarTopicosMqtt() {
  if ((millis() - ultimoPushMqtt) > intervaloPushMqtt) {
    if (ultimaTemperatura != ultimaTempEnviada || ultimaHumedad != ultimaHumEnviada) {
      char mensajeMqtt[MSG_MAX_LENGTH];
      snprintf(mensajeMqtt, MSG_MAX_LENGTH, "{\"temperatura\":%.2f,\"humedad\":%.2f}", ultimaTemperatura, ultimaHumedad);
      mqttClient.publish("esp32/sensores", mensajeMqtt);
      ultimaTempEnviada = ultimaTemperatura;
      ultimaHumEnviada = ultimaHumedad;
    }
    ultimoPushMqtt = millis();
  }
}
// ---------------------------------


void setup() {
  Serial.begin(115200);
  Serial.println("SETUP: start ---------------------------");
  motor.init(MOTOR_DEFAULT_ESTADO, MOTOR_DURACION_MS);
  tempSensor.init();
  Serial.print("DEBUG conexión WiFi (esperado false): ");
  Serial.println(wifiConn.isConnected() ? "true" : "false");
  wifiConn.connect(WIFI_SSID, WIFI_PASSWORD);
  ultimaNotificacionBot = 0;
  telegramBot.setChatIdsValidos({GROUP_CHAT_ID, CHAT_ID_1, CHAT_ID_2});
  telegramBot.registerCommand("/ayuda", comandoAyuda);
  telegramBot.registerCommand("/abrir", comandoAbrirVentana);
  telegramBot.registerCommand("/cerrar", comandoCerrarVentana);
  telegramBot.registerCommand("/estado", comandoInformarEstado);
  mqttClient.init(MQTT_BROKER_ADRESS, MQTT_PORT);
  Serial.println("SETUP: end ---------------------------");
}

void loop() {
  Serial.println("LOOP: start ---------------------------");
  checkMqttConnection();
  checkTemperatura();
  checkHumedad();
  printTemperatura();
  printHumedad();
  printVentanas();
  checkMotor();
  checkTelegramBot();
  publicarTopicosMqtt();
  delay(LOOP_DELAY);
  Serial.println("LOOP: end ---------------------------");
}
