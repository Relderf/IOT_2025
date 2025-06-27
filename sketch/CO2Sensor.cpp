#include "CO2Sensor.h"
#include <env.h>
#include <Arduino.h>
#include "config.h"

#define DHTTYPE DHT22

static bool papasPudriendose = false;
static unsigned long ultimoCambioPudricion = 0;
static bool ventilacionActiva = false;

CO2Sensor::CO2Sensor(int p, unsigned long intervalo) {
    intervaloLecturaCO2Ms = intervalo;
    ultimaLecturaCO2Ms = 0;
    ultimoCO2 = NAN;
}

void CO2Sensor::init() {}

bool CO2Sensor::CO2Valida() {
    return !isnan(ultimoCO2);
}

float randomCO2() {
    return random(CO2_MIN_SIMULADO, CO2_MAX_SIMULADO + 1);
}

float simularCO2() {
    static float CO2Simulado = 0;
    static float objetivo = 0;
    static unsigned long ultimaLecturaMs = 0;

    if ((millis() - ultimoCambioPudricion) > (60000 + random(0, 60000))) {
        papasPudriendose = random(0, 2);
        ultimoCambioPudricion = millis();
    }

    float factorPudricion = papasPudriendose ? 2.0 : 1.0;
    float factorVentilacion = ventilacionActiva ? 0.5 : 1.0;
    float baseCO2 = 400.0 * KGS_PAPAS * factorPudricion * factorVentilacion * M3;
    float maxCO2 = 1200.0 * KGS_PAPAS * factorPudricion * factorVentilacion * M3;

    if ((millis() - ultimaLecturaMs) > 5000) {
        ultimaLecturaMs = millis();
        if (CO2Simulado == 0) {
            CO2Simulado = baseCO2;
            objetivo = maxCO2;
        }
        if (abs(CO2Simulado - objetivo) < 10.0) {
            objetivo = baseCO2 + random(0, (int)(maxCO2 - baseCO2));
        } else {
            float paso = random(0, 21) / 10.0;
            if (CO2Simulado < objetivo) {
                CO2Simulado += paso;
                if (CO2Simulado > objetivo) CO2Simulado = objetivo;
            } else {
                CO2Simulado -= paso;
                if (CO2Simulado < objetivo) CO2Simulado = objetivo;
            }
        }
    }
    return CO2Simulado;
}

void CO2Sensor::setVentilacion(bool activa) {
    ventilacionActiva = activa;
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
