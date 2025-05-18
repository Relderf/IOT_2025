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


// Módulo Ventanas.
// ---------------------------------
boolean ventanasAbiertas = VENTANAS_DEFAULT_ESTADO;

bool ventanasEnUso() {
  return motor.estaEncendido();
}

void abrirVentanas() {
  if (!ventanasEnUso() && !ventanasAbiertas) {
    Serial.println("Ventanas: abriendo...");
    motor.encender();
    ventanasAbiertas = true;
  }
}

void cerrarVentanas() {
  if (!ventanasEnUso() && ventanasAbiertas) {
    Serial.println("Ventanas: cerrando...");
    motor.encender();
    ventanasAbiertas = false;
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
    if (modoAutomatico && (ultimaTemperatura > TEMP_ALTA)) {
      abrirVentanas();
    } else if (modoAutomatico && (ultimaTemperatura < TEMP_BAJA)) {
      cerrarVentanas();
    }
  }
}

void printTemperatura() {
  if (tempSensor.temperaturaValida()) {
    Serial.println("Temperatura: " + String(tempSensor.getTemperatura()) + " °C.");
  }
}
// ---------------------------------


// Módulo WiFi.
// ---------------------------------
#include "WifiConn.h"
WifiConn wifiConn;
// ---------------------------------


// Módulo Bot Telegram.
// ---------------------------------
#include "TelegramBot.h"
const unsigned long intervaloNotificacioBot = BOT_INTERVALO_NOTIFICACION;
unsigned long ultimaNotificacionBot;
TelegramBot telegramBot(TOKEN_BOT, wifiConn.getCliente(), BOT_INTERVALO_CHEQUEO_MENSAJES);

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
  Serial.println("TelegramBot: Desctivando modo automático...");
  telegramBot.sendMessage(chatId, "Modo automático: DESACTIVADO.");
  modoAutomatico = false;
}

void comandoAbrirVentana(String chatId) {
  Serial.println("TelegramBot: Abriendo ventana...");
  comandoDesactivarModoAutomatico(chatId);
  telegramBot.sendMessage(chatId, "Abriendo ventanas...");
  abrirVentanas();
}

void comandoCerrarVentana(String chatId) {
  Serial.println("TelegramBot: Cerrando ventana...");
  comandoDesactivarModoAutomatico(chatId);
  telegramBot.sendMessage(chatId, "Cerrando ventanas...");
  cerrarVentanas();
}

void comandoInformarEstado(String chatId) {
  Serial.println("TelegramBot: Informando estado...");
  String estado = "Temperatura: " + String(ultimaTemperatura) + " °C.\n";
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
  if ((millis() - ultimaNotificacionBot) > intervaloNotificacioBot) {
    comandoInformarEstado(GROUP_CHAT_ID);
    ultimaNotificacionBot = millis();
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
  Serial.println("SETUP: end ---------------------------");
}

void loop() {
  Serial.println("LOOP: start ---------------------------");
  checkTemperatura();
  printTemperatura();
  printVentanas();
  checkMotor();
  checkTelegramBot();
  delay(LOOP_DELAY);
  Serial.println("LOOP: END ---------------------------");
}
