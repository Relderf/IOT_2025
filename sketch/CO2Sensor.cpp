#include "CO2Sensor.h"
#include <env.h>
#include <Arduino.h>
#include "config.h"

#define DHTTYPE DHT22

static float CO2Simulado = 0.0;
static float papasNormales = KGS_PAPAS;
static float papasPodridas = 0.0;
static float m3 = M3; // Volumen de la cámara en m³
static unsigned long ultimaLecturaMs = 0;
static unsigned long ultimoCambioPudricion = 0;
bool ventilacionActiva = false; // Estado de la ventilación, TRUE = prendida; FALSE = apagada.

CO2Sensor::CO2Sensor(int p, unsigned long intervalo) {
    ultimoCO2 = 0.0;
    ultimaLecturaCO2Ms = 0;
    intervaloLecturaCO2Ms = intervalo;
}

void CO2Sensor::init() {}

bool CO2Sensor::CO2Valida() {
    return !isnan(ultimoCO2);
}

float simularCO2() {
    unsigned long ahora = millis();

    // Solo actualizar cada vuelta del loop (ajusta si querés menos frecuente)
    if (ahora - ultimaLecturaMs > 1000) {
        ultimaLecturaMs = ahora;

        // Pudrición: 0-2% de las papas se pudren por vuelta
        if (papasPodridas < KGS_PAPAS) {
            papasPodridas = papasPodridas + KGS_PAPAS * (random(0, 200)/100.0);
            papasNormales = KGS_PAPAS - papasPodridas;
            if (papasNormales < 0) {
                papasNormales = 0;
            }
            if (papasPodridas > KGS_PAPAS) {
                papasPodridas = KGS_PAPAS; // No más de 100% de papas podridas
            }
        }

        // Incremento de CO2 por respiración de papas normales y podridas
        CO2Simulado = CO2Simulado + (((papasNormales * CO2_PPM_S) 
                                    +(papasPodridas * CO2_PPM_S * FACTOR_CO2_PODRIDAS))
                                    /m3);

        // Si ventilación activa, baja CO2
        if (ventilacionActiva) {
            CO2Simulado -= 150.0;
            if (CO2Simulado < 0.0) {
                CO2Simulado = 0.0;
            }
        }
    }
    return CO2Simulado;
}

void CO2Sensor::checkCO2() {
    if ((millis() - ultimaLecturaCO2Ms) > intervaloLecturaCO2Ms) {
        ultimaLecturaCO2Ms = millis();
        float CO2;
        CO2 = simularCO2();
        if (!isnan(CO2)) {
            ultimoCO2 = CO2;
        }
    }
}

float CO2Sensor::getCO2() {
    checkCO2();
    return ultimoCO2;
}

void CO2Sensor::setPapasNormales(float kilos) {
    if (kilos >= 0) {
        papasNormales = kilos;
        papasPodridas = 0.0; // Reiniciar papas podridas al cambiar cantidad normal
        Serial.println("Kilos de papas actualizados: " + String(kilos));
    } else {
        Serial.println("Valor inválido para kilos: " + String(kilos));
    }
}

void CO2Sensor::setM3(float nuevoM3) {
    if (m3 > 0) {
        m3 = nuevoM3;
        Serial.printf("Nuevo volumen de la cámara: %.2fm³.\n", m3);
    } else {
        Serial.println("Volumen inválido para la cámara.");
    }
}

void CO2Sensor::setVentilacion(bool activa) {
    ventilacionActiva = activa;
}

bool CO2Sensor::getVentilacion() {
    return ventilacionActiva;
}
