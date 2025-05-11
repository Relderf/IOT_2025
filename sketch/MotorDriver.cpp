#include "MotorDriver.h"
#include <Arduino.h>

MotorDriver::MotorDriver(int p) {
    pin = p;
    pinMode(pin, OUTPUT);
}

void MotorDriver::init(bool estado, int duracion) {
    Serial.println("Motor inicializado.");
    duracionEncendidoMs = duracion;
    tiempoInicioEncendidoMs = 0;
    encendido = !estado;
    if (estado) {
      encender();
    } else {
      apagar();
    }
}

void MotorDriver::encender() {
    if (!estaEncendido()) {
        Serial.println("Motor encendido.");
        tiempoInicioEncendidoMs = millis();
        encendido = true;
        digitalWrite(pin, HIGH);
    }
}

void MotorDriver::apagar() {
    if (estaEncendido()) {
        Serial.println("Motor apagado.");
        tiempoInicioEncendidoMs = 0;
        encendido = false;
        digitalWrite(pin, LOW);
    }
}

bool MotorDriver::estaEncendido() {
    return encendido;
}

void MotorDriver::loopUpdate() {
    if (estaEncendido()) {
        unsigned long tiempoTranscurridoMs = millis() - tiempoInicioEncendidoMs;
        unsigned long tiempoRestanteMs = max((unsigned long) 0, duracionEncendidoMs - tiempoTranscurridoMs);
        Serial.print("Motor activo, quedan ");
        Serial.print(tiempoRestanteMs);
        Serial.println(" ms.");
        if (tiempoTranscurridoMs >= duracionEncendidoMs) {
            apagar();
        }
    }
}
