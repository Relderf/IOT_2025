#include "TempSensor.h"
#include <DHT.h>
#include <env.h>
#include "config.h"

#define DHTTYPE DHT22

TempSensor::TempSensor(int p, unsigned long intervalo) : dht(p, DHTTYPE) {
    intervaloLecturaTempMs = intervalo;
    ultimaLecturaTempMs = 0;
    ultimaTemperatura = NAN;
}

void TempSensor::init() {
    dht.begin();
}

bool TempSensor::temperaturaValida() {
    return !isnan(ultimaTemperatura);
}

float randomTemp() {
    return random(TEMP_MIN_SIMULADA, TEMP_MAX_SIMULADA + 1);
}

float simularTemperatura() {
    static float temperaturaSimulada = 0;
    static float objetivo = 0;
    static unsigned long ultimaLecturaMs = 0;

    if ((millis() - ultimaLecturaMs) > 5000) {
        ultimaLecturaMs = millis();

        if (temperaturaSimulada == 0) {
            temperaturaSimulada = randomTemp();
            objetivo = randomTemp();
        }

        if (abs(temperaturaSimulada - objetivo) < 0.5) {
            objetivo = randomTemp();
        } else {
            float paso = random(0, 21) / 10.0; // Paso de entre 0 y 2

            if (temperaturaSimulada < objetivo) {
                temperaturaSimulada += paso;
                if (temperaturaSimulada > objetivo) temperaturaSimulada = objetivo;
            } else {
                temperaturaSimulada -= paso;
                if (temperaturaSimulada < objetivo) temperaturaSimulada = objetivo;
            }
        }
    }
    return temperaturaSimulada;
}

void TempSensor::checkTemperatura() {
    if ((millis() - ultimaLecturaTempMs) > intervaloLecturaTempMs) {
        ultimaLecturaTempMs = millis();
 		float temperatura;
        if (TEMP_MOCK) {
            temperatura = simularTemperatura();
        } else {
            temperatura = dht.readTemperature();
        }
        if (!isnan(temperatura)) {
            ultimaTemperatura = temperatura;
        }
    }
}

float TempSensor::getTemperatura() {
    checkTemperatura();
    return ultimaTemperatura;
}
