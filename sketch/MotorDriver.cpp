#include "MotorDriver.h"
#include <Arduino.h>

MotorDriver::MotorDriver(int p) {
    pin = p;
    pinMode(pin, OUTPUT);
}

void MotorDriver::init(bool estado, int duracion) {
    Serial.print("Motor: inicializado. Duración encendido: ");
    duracionEncendidoMs = duracion;
    Serial.print(duracionEncendidoMs);
    tiempoInicioEncendidoMs = 0;
    encendido = !estado;
    Serial.print("ms. Estado: ");
    Serial.print(estado ? "Encendido" : "Apagado");
    Serial.println(".");
    if (estado) {
      encender();
    } else {
      apagar();
    }
}

void MotorDriver::encender() {
    if (!estaEncendido()) {
        Serial.print("Motor: encendeido. Inicio: ");
        tiempoInicioEncendidoMs = millis();
        Serial.print(tiempoInicioEncendidoMs);
        Serial.println("ms.");
        encendido = true;
        digitalWrite(pin, HIGH);
    }
}

void MotorDriver::apagar() {
    if (estaEncendido()) {
        Serial.println("Motor: apagado.");
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
        long tiempoRestanteMs = max((long)0, duracionEncendidoMs - (long)tiempoTranscurridoMs);
        Serial.print("Motor: update. Tiempo encendido: ");
        Serial.print(tiempoTranscurridoMs);
        Serial.print(" ms. Tiempo hasta apagado: ");
        Serial.print(tiempoRestanteMs);
        Serial.println("ms.");
        if (tiempoTranscurridoMs >= duracionEncendidoMs) {
            apagar();
        }
    }
}
