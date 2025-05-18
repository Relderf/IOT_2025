#include "TempSensor.h"
#include <DHT.h>

#define DHTTYPE DHT22

TempSensor::TempSensor(int p, unsigned long intervalo) : dht(p, DHTTYPE), mock(true) {
    intervaloLecturaTempMs = intervalo;
    ultimaLecturaTempMs = 0;
    ultimaTemperatura = NAN;
    bool mock = mock;
}

void TempSensor::init() {
    dht.begin();
}

bool TempSensor::temperaturaValida() {
    return !isnan(ultimaTemperatura);
}

void TempSensor::checkTemperatura() {
    if ((millis() - ultimaLecturaTempMs) > intervaloLecturaTempMs) {
        ultimaLecturaTempMs = millis();
        float temperatura = dht.readTemperature();
        if (!isnan(temperatura)) {
            ultimaTemperatura = temperatura;
        }
    }
}

float TempSensor::getTemperatura() {
    if (mock) {
        ultimaTemperatura = random(18, 36);
    } else {
        checkTemperatura();
    }
    return ultimaTemperatura;
}
