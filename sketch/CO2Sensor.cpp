#include "CO2Sensor.h"
#include <env.h>
#include "config.h"
#include <Arduino.h>

#define DHTTYPE DHT22

CO2Sensor::CO2Sensor(int p, unsigned long intervalo) : dht(p, DHTTYPE) {
    intervaloLecturaCO2Ms = intervalo;
    ultimaLecturaCO2Ms = 0;
    ultimoCO2 = 0.0;
}

void CO2Sensor::init() {
}

float randomCO2() {
    return random(CO2_MIN_SIMULADO, CO2_MAX_SIMULADO + 1);
}

void CO2Sensor::checkCO2() {
    static float CO2Simulado = 0;
    static float objetivo = 0;
    static unsigned long ultimaLecturaMs = 0;
    if ((millis() - ultimaLecturaCO2Ms) > intervaloLecturaCO2Ms) {

        ultimaLecturaMs = millis();

        if (CO2Simulado == 0) {
            CO2Simulado = randomCO2();
            objetivo = randomCO2();
        }

        if (abs(CO2Simulado - objetivo) < 0.5) {
            objetivo = randomCO2();
        } else {
            float paso = random(0, 21) / 10.0; // Paso de entre 0 y 2
            if (CO2Simulado < objetivo) {
                CO2Simulado += paso;
                if (CO2Simulado > objetivo) CO2Simulado = objetivo;
            } else {
                CO2Simulado -= paso;
                if (CO2Simulado < objetivo) CO2Simulado = objetivo;
            }
        }
    }
    ultimoCO2 = CO2Simulado;
    return CO2Simulado;
}

float CO2Sensor::getCO2() {
    checkCO2();
    return ultimoCO2;
}
