#include <DHT.h>

#define DHTPIN 15
#define DHTTYPE DHT22
#define LEDPIN 2

DHT dht(DHTPIN, DHTTYPE);

boolean ventanaAbierta = false;
int motorCountdown = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);
}

void loop() {
  float temp = dht.readTemperature();
  if (isnan(temp)) {
    Serial.println("Error leyendo el sensor DHT!");
  } else {
    Serial.print("Temperatura: ");
    Serial.print(temp);
    Serial.print(" °C - Ventana: ");
    if (motorCountdown == 0) {
      Serial.println(ventanaAbierta ? "Abierta" : "Cerrada");
    } else {
      Serial.println(ventanaAbierta ? "Abriendo..." : "Cerrando...");
    }

    if (motorCountdown == 0) {
      if ((temp > 30.0) && (!ventanaAbierta)) {
        Serial.println("Temperatura ALTA -> Abriendo ventana...");
        encenderMotor();
        ventanaAbierta = true;
      } else if ((temp < 20.0) && (ventanaAbierta)) {
        Serial.println("Temperatura BAJA -> Cerrando ventana...");
        encenderMotor();
        ventanaAbierta = false;
      }
    }
  }

  if (motorCountdown > 0) {
    Serial.print("Motor activo, quedan ");
    Serial.print(motorCountdown);
    Serial.println(" segundos.");
    motorCountdown--;
    if (motorCountdown == 0) {
      apagarMotor();
    }
  }

  delay(1000);
}

void encenderMotor() {
  Serial.println("Motor encendido.");
  digitalWrite(LEDPIN, HIGH);
  motorCountdown = 10;
}

void apagarMotor() {
  digitalWrite(LEDPIN, LOW);
  Serial.println("Motor apagado.");
}