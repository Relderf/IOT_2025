#include "MotorDriver.h"
#include "config.h"
#include <Arduino.h>

MotorDriver motor(MOTOR_PIN);

MotorDriver::MotorDriver(int p) {
    pin = p;
    pinMode(pin, OUTPUT);
}

void MotorDriver::init(bool estado, int duracion) {
    Serial.print("Motor: inicializado. Duración encendido: ");
    duracionEncendidoMs = duracion;
    Serial.print(duracionEncendidoMs);
    tiempoInicioEncendidoMs = 0;
    ventilacionPrendida = VENTILACION_DEFAULT_ESTADO;
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
        Serial.print("Motor: encendido. Inicio: ");
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

void MotorDriver::prenderVentilacion() {
  if (!estaEncendido() && !getEstadoVentilacion()) {
    encender();
    ventilacionPrendida = true;
  }
}

void MotorDriver::apagarVentilacion() {
  if (!estaEncendido() && getEstadoVentilacion()) {
    encender();
    ventilacionPrendida = false;
  }
}

bool MotorDriver::puedePrenderVentilacion() {
    return !estaEncendido() && !ventilacionPrendida;
}

bool MotorDriver::getEstadoVentilacion() {
  return ventilacionPrendida;
}