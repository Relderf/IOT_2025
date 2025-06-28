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
    static float CO2Simulado = 400.0 * KGS_PAPAS;
    static bool papasPudriendose = false;
    static unsigned long ultimaLecturaMs = 0;
    unsigned long ahora = millis();

    // Solo actualizar cada vuelta del loop (ajusta si querés menos frecuente)
    if (ahora - ultimaLecturaMs > 1000) {
        ultimaLecturaMs = ahora;

        // Pudrición: 3% de chance por vuelta si no están podridas
        if (!papasPudriendose && random(0, 100) < 3) {
            papasPudriendose = true;
            Serial.println("Papas pudriéndose detectadas.");
        }

        // Si ventilación activa, baja CO2
        if (ventilacionActiva) {
            CO2Simulado -= 150.0;
            if (CO2Simulado < 400.0 * KGS_PAPAS) {
                CO2Simulado = 400.0 * KGS_PAPAS;
                papasPudriendose = false; // Se simula que las papas fueron retiradas
                Serial.println("Papas retiradas, CO2 normalizado.");
            }
        } else {
            // Sube CO2 según estado de pudrición
            float factor = papasPudriendose ? 0.10 : 0.02; // 10% o 2% por vuelta
            float subida = CO2Simulado * factor;
            if (subida < 1.0) subida = 1.0; // Siempre sube al menos 1
            CO2Simulado += subida;
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
