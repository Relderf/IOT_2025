#include "env.h"
#include "config.h"


// Módulo Motor.
// ---------------------------------
#include "MotorDriver.h"
MotorDriver motor(MOTOR_PIN);

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
boolean ventanasAbiertas = VENTANAS_DEFAULT_ESTADO;

bool ventanasEnUso() {
  return motor.estaEncendido();
}

void abrirVentanas(String chatId = "") {
  if (!ventanasEnUso() && !ventanasAbiertas) {
    Serial.println("Ventanas: abriendo...");
    motor.encender();
    ventanasAbiertas = true;
    if (chatId != "") {
      telegramBot.sendMessage(chatId, "Abriendo ventanas...");
    }
  }
}

void cerrarVentanas(String chatId = "") {
  if (chatId != "") {
    if (!ventanasEnUso() && ventanasAbiertas) {
      Serial.println("Ventanas: cerrando...");
      motor.encender();
      ventanasAbiertas = false;
      telegramBot.sendMessage(chatId, "Cerrando ventanas...");
    } else {
      telegramBot.sendMessage(chatId, "Ventanas ya cerradas o en uso.");
    }
  }
}

void printVentanas() {
  Serial.print("Ventanas: ");
  if (ventanasEnUso()) {
    Serial.println(ventanasAbiertas ? "Abriendo..." : "Cerrando...");
  } else {
    Serial.println(ventanasAbiertas ? "Abiertas." : "Cerradas.");
  }
}
// ---------------------------------


// Módulo Temperatura.
// ---------------------------------
#include "TempSensor.h"
TempSensor tempSensor(TEMP_SENSOR_PIN, TEMP_SENSOR_DELAY);
float ultimaTemperatura;
bool modoAutomatico = VENTANAS_MODO_AUTOMATICO;

void checkTemperatura(bool forzar = false) {
  float nuevaTemperatura = tempSensor.getTemperatura();
  if (!tempSensor.temperaturaValida()) {
    Serial.println("Error al obtener la temperatura.");
    return;
  }
  if (forzar || (ultimaTemperatura != nuevaTemperatura)) {
    ultimaTemperatura = nuevaTemperatura;
    if (!ventanasEnUso && modoAutomatico) {
      if (ultimaTemperatura > TEMP_ALTA) {
        telegramBot.sendMessage(GROUP_CHAT_ID, "Detectado Temp. ALTA: " + String(ultimaTemperatura) + " °C.");
        abrirVentanas(GROUP_CHAT_ID);
      } else if (ultimaTemperatura < TEMP_BAJA) {
        telegramBot.sendMessage(GROUP_CHAT_ID, "Detectado Temp. BAJA: " + String(ultimaTemperatura) + " °C.");
        cerrarVentanas(GROUP_CHAT_ID);
      }
    }
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
float ultimaHumedad = 0;

void checkHumedad() {
  ultimaHumedad = random (30,95);
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

void comandoActivarModoAutomatico(String chatId) {
  Serial.println("TelegramBot: Activando modo automático...");
  telegramBot.sendMessage(chatId, "Modo automático: ACTIVADO.");
  modoAutomatico = true;
  checkTemperatura(true);
}

void comandoDesactivarModoAutomatico(String chatId) {
  String msj_terminal;
  if (modoAutomatico) {
    modoAutomatico = false;
    msj_terminal = "TelegramBot: Desctivando modo automático...";
    telegramBot.sendMessage(chatId, "Modo automático: DESACTIVADO.");
  } else {
    msj_terminal = "TelegramBot: El modo automático ya se encuentra desactivado.";
  }
  Serial.println(msj_terminal);
}

void comandoAbrirVentana(String chatId) {
  Serial.println("TelegramBot: Abriendo ventana...");
  comandoDesactivarModoAutomatico(chatId);
  abrirVentanas(chatId);
}

void comandoCerrarVentana(String chatId) {
  Serial.println("TelegramBot: Cerrando ventana...");
  comandoDesactivarModoAutomatico(chatId);
  cerrarVentanas(chatId);
}

void comandoInformarEstado(String chatId) {
  Serial.println("TelegramBot: Informando estado...");
  String estado = "Temperatura: " + String(ultimaTemperatura) + " °C.\n";
  estado += "Modo automatico: " + String((modoAutomatico ? "Activado" : "Desactivado")) + ".\n";
  String estadoVentanas;
  if (ventanasEnUso()) {
    estadoVentanas = (ventanasAbiertas ? "Abriendo..." : "Cerrando...");
  } else {
    estadoVentanas = (ventanasAbiertas ? "Abiertas." : "Cerradas.");
  }
  estado += ("Ventanas: " + estadoVentanas);
  telegramBot.sendMessage(chatId, estado);
}

void checkTelegramBot() {
  telegramBot.checkMensajes();
  if (modoAutomatico  && ((millis() - ultimaNotificacionBot) > intervaloNotificacioBot)) {
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

void publicarTopicosMqtt() {
  if ((millis() - ultimoPushMqtt) > intervaloPushMqtt) {
    char tempString[8];
    dtostrf(ultimaTemperatura, 1, 2, tempString);
    mqttClient.publish("esp32/temperatura", tempString);
    char humString[8];
    dtostrf(ultimaHumedad, 1, 2, humString);
    mqttClient.publish("esp32/humedad", humString);
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
  telegramBot.registerCommand("/activar", comandoActivarModoAutomatico);
  telegramBot.registerCommand("/desactivar", comandoDesactivarModoAutomatico);
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
