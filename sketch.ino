// Led
#include <DHT.h>
#define DHTPIN 15
#define DHTTYPE DHT22
#define LEDPIN 2
DHT dht(DHTPIN, DHTTYPE);

// Wifi
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
const char* ssid = "Wokwi-GUEST";
const char* password = "";
WiFiClientSecure clienteWifi;

// Telegram
#include <UniversalTelegramBot.h>
#include <env.h>

UniversalTelegramBot bot(TOKEN_BOT, clienteWifi);
unsigned long ultimaNotificacion = 0;
const unsigned long intervaloNotificacion = 5000;

boolean ventanaAbierta = false;
boolean modoAutomatico = true;
unsigned long tiempoEncendidoMotor = 0;
float temp;
unsigned long ultimaLecturaTemp = 0;
const unsigned long intervaloLecturaTemp = 2000;
const unsigned long duracionMotor = 10000;

// Frecuencia de chequeo de mensajes de Telegram
int delayBot = 1000;
unsigned long ultimoChequeoBot;

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Conectarse a wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  clienteWifi.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  WiFi.setSleep(false);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Imprime la IP del ESP32
  Serial.println(WiFi.localIP());

  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);
}

void loop() {
  if (millis() - ultimaLecturaTemp > intervaloLecturaTemp){
    ultimaLecturaTemp = millis();
    temp = dht.readTemperature();

    if (isnan(temp)) {
      Serial.println("Error leyendo la temperatura.");
    } else {
      Serial.print("Temperatura: ");
      Serial.print(temp);
      Serial.print(" °C - Ventana: ");
      if (tiempoEncendidoMotor == 0) {
        Serial.println(ventanaAbierta ? "Abierta" : "Cerrada");
      } else {
        Serial.println(ventanaAbierta ? "Abriendo..." : "Cerrando...");
      }

      if (tiempoEncendidoMotor == 0) {
        if ((temp > 30.0) && (!ventanaAbierta)) {
          Serial.println("Temperatura ALTA -> Abriendo ventana...");
          abrirVentana();
        } else if ((temp < 20.0) && (ventanaAbierta)) {
          Serial.println("Temperatura BAJA -> Cerrando ventana...");
          cerrarVentana();
        }
      }
    }
  }

  if (tiempoEncendidoMotor > 0) {
    unsigned long tiempoRestante = duracionMotor - (millis() - tiempoEncendidoMotor);
    Serial.print("Motor activo, quedan ");
    Serial.print(tiempoRestante / 1000);
    Serial.println(" segundos.");
    if (millis() - tiempoEncendidoMotor > duracionMotor) {
      apagarMotor();
    }
  }

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

  delay(100);
}

void encenderMotor() {
  Serial.println("Motor encendido.");
  digitalWrite(LEDPIN, HIGH);
  tiempoEncendidoMotor = millis();
}

void apagarMotor() {
  digitalWrite(LEDPIN, LOW);
  Serial.println("Motor apagado.");
  tiempoEncendidoMotor = 0;
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

void abrirVentana() {
  encenderMotor();
  ventanaAbierta = true;
}

void cerrarVentana() {
  encenderMotor();
  ventanaAbierta = true;
}

void comandoAbrirVentana(String chat_id) {
  comandoDesactivarModoAutomatico(chat_id);
  abrirVentana();
  bot.sendMessage(chat_id, "Abriendo ventana...", "");
  Serial.println("Abriendo ventana...");
}

void comandoCerrarVentana(String chat_id) {
  comandoDesactivarModoAutomatico(chat_id);
  cerrarVentana();
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
  String estado = "Temperatura: " + String(temp) + "\n";
  String estadoVentana = ventanaAbierta ? "abiertas." : "cerradas.";
  estado += ("Ventanas: " + estadoVentana);
  bot.sendMessage(chat_id, estado, "");
}
