#include "TempSensor.h"
#include <DHT.h>

#define DHTTYPE DHT22

TempSensor::TempSensor(int p, unsigned long intervalo, bool mockMode) : dht(p, DHTTYPE), mock(mockMode) {
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

void TempSensor::checkTemperatura() {
    if ((millis() - ultimaLecturaTempMs) > intervaloLecturaTempMs) {
        ultimaLecturaTempMs = millis();
 		float temperatura;
        if (mock) {
            temperatura = random(18, 36);
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
