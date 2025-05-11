#include "env.h"
#include "config.h"


// Módulo Motor.
// ---------------------------------
#include "MotorDriver.h"
MotorDriver motor(MOTOR_PIN);
// ---------------------------------


// Módulo Ventanas.
// ---------------------------------
boolean ventanasAbiertas = VENTANAS_DEFAULT_ESTADO;

bool ventanasEnUso() {
  return motor.estaEncendido();
}

void abrirVentanas() {
  if (!ventanasEnUso() && !ventanasAbiertas) {
    Serial.println("Abriendo ventanas...");
    motor.encender();
    ventanasAbiertas = true;
  }
}

void cerrarVentanas() {
  if (!ventanasEnUso() && ventanasAbiertas) {
    Serial.println("Cerrando ventanas...");
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
TempSensor tempSensor(TEMP_SENSOR_PIN);
const unsigned long intervaloLecturaTempMs = TEMP_SENSOR_DELAY;
unsigned long ultimaLecturaTempMs = 0;
float temperatura;

bool temperaturaValida() {
  return !isnan(temperatura);
}

void checkTemperatura() {
  if ((millis() - ultimaLecturaTempMs) > intervaloLecturaTempMs) {
    ultimaLecturaTempMs = millis();
    temperatura = tempSensor.getTemperatura();
    if (!temperaturaValida()) {
      Serial.println("Error al obtener la temperatura.");
    } else {
      if (temperatura > TEMP_ALTA) {
        abrirVentanas();
      } else if (temperatura < TEMP_BAJA) {
        cerrarVentanas();
      }
    }
  }
}

void printTemperatura() {
  if (temperaturaValida()) {
    Serial.println("Temperatura: " + String(temperatura) + " °C.");
  }
}
// ---------------------------------


// Módulo WiFi.
// ---------------------------------
#include "WifiConn.h"
WifiConn wifiConn;
// ---------------------------------


// Telegram
// ---------------------------------
#include <UniversalTelegramBot.h>
UniversalTelegramBot bot(TOKEN_BOT, wifiConn.getCliente());
unsigned long ultimaNotificacion = 0;
const unsigned long intervaloNotificacion = 5000;
// Frecuencia de chequeo de mensajes de Telegram
int delayBot = 1000;
unsigned long ultimoChequeoBot;
// ---------------------------------


boolean modoAutomatico = true;

void setup() {
  Serial.begin(115200);

  // Motor setup.
  motor.init(MOTOR_DEFAULT_ESTADO, MOTOR_DURACION_MS);

  // Sensor Temp. setup.
  tempSensor.init();

  // WiFi setup.
  wifiConn.getCliente().setCACert(TELEGRAM_CERTIFICATE_ROOT);
  wifiConn.connect(WIFI_SSID, WIFI_PASSWORD);
}

void loop() {
  checkTemperatura();
  printTemperatura();
  printVentanas();

  if (millis() > ultimoChequeoBot + delayBot)  {
    int cantNuevosMensajes = bot.getUpdates(bot.last_message_received + 1);
    while(cantNuevosMensajes) {
      Serial.println("Nuevo mensaje recibido.");
      recibirMensajes(cantNuevosMensajes);
      cantNuevosMensajes = bot.getUpdates(bot.last_message_received + 1);
    }
    ultimoChequeoBot = millis();
  }

  if (modoAutomatico 
  && ((millis() - ultimaNotificacion) > intervaloNotificacion)) {
    informarEstado(GROUP_ID);
    ultimaNotificacion = millis();
  }

  motor.loopUpdate();

  delay(100);
}



void recibirMensajes(int cantNuevosMensajes) {
  
  Serial.print("Recibiendo mensajes: ");
  Serial.println(String(cantNuevosMensajes));

  for (int i=0; i<cantNuevosMensajes; i++) {
    // ID de quien envía el mensaje
    String chat_id = String(bot.messages[i].chat_id);

    // Si no está acreditado
    if (chat_id != CHAT_ID_1 && chat_id != CHAT_ID_2 && chat_id != GROUP_ID){
      Serial.println("Chat ID recibido: " + String(chat_id));
      bot.sendMessage(chat_id, "Acceso denegado. Usuario no autorizado.", "");
      continue;
    }
    // Imprimir el mensaje recibido
    String texto = bot.messages[i].text;
    Serial.println(texto);
    String nombreUsuario = bot.messages[i].from_name;
    
    if (texto == "/ayuda") {
      String welcome = "Hola, " + nombreUsuario + ".\n";
      welcome += "Usar los siguientes comandos para controlar.\n\n";
      welcome += "/activar para dejar andando el modo automático \n";
      welcome += "/desactivar para apagar el modo automático \n";
      welcome += "/abrir para abrir las ventanas \n";
      welcome += "/cerrar para cerrar las ventanas \n";
      welcome += "/estado para ver la temperatura y el estado actual de las ventanas \n";
      bot.sendMessage(chat_id, welcome, "");
    }
    else if (texto == "/activar") {
      comandoActivarModoAutomatico(chat_id);
    }
    else if (texto == "/desactivar") {
      comandoDesactivarModoAutomatico(chat_id);
    }
    else if (texto == "/abrir") {
      comandoAbrirVentana(chat_id);
    }
    else if (texto == "/cerrar") {
      comandoCerrarVentana(chat_id);
    }
    else if (texto == "/estado") {
      informarEstado(chat_id);
    }
    else {
      bot.sendMessage(chat_id, "Comando no encontrado.", "");
    }
  }
}

void comandoAbrirVentana(String chat_id) {
  comandoDesactivarModoAutomatico(chat_id);
  abrirVentanas();
  bot.sendMessage(chat_id, "Abriendo ventana...", "");
  Serial.println("Abriendo ventana...");
}

void comandoCerrarVentana(String chat_id) {
  comandoDesactivarModoAutomatico(chat_id);
  cerrarVentanas();
  bot.sendMessage(chat_id, "Cerrando ventana...", "");
  Serial.println("Cerrando ventana...");
}

void comandoActivarModoAutomatico(String chat_id) {
  modoAutomatico = true;
  bot.sendMessage(chat_id, "Modo automático: ACTIVADO.", "");
  Serial.println("Activando modo automático...");
}

void comandoDesactivarModoAutomatico(String chat_id) {
  modoAutomatico = false;
  bot.sendMessage(chat_id, "Modo automático: DESACTIVADO.", "");
  Serial.println("Desctivando modo automático...");
}

void informarEstado(String chat_id) {
  String estado = "Temperatura: " + String(temperatura) + "\n";
  String estadoVentanas = ventanasAbiertas ? "abiertas." : "cerradas.";
  estado += ("Ventanas: " + estadoVentanas);
  bot.sendMessage(chat_id, estado, "");
}
