#include "TempSensor.h"
#include <DHT.h>
#include <env.h>

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

float simularTemperatura() {
    // La primera vez que se llama a esta función, se simula una temperatura 
    //aleatoria entre 10 y 35 grados Celsius. Despues de eso, crece hacia 
    //el extremo que tenga más lejos (10 o 35) de a 1 cada 5 segundos, 
    //y cuando llega al límite, vuelve para el otro lado.
    static float temperaturaSimulada = 0;
    static unsigned long ultimaLecturaSimuladaMs = 0;
    static bool creciendo = true;
    if ((millis() - ultimaLecturaSimuladaMs) > 5000) {
        ultimaLecturaSimuladaMs = millis();
        if (temperaturaSimulada == 0) {
            temperaturaSimulada = random(10, 35);
        } else {
            if (creciendo) {
                temperaturaSimulada += 1;
                if (temperaturaSimulada >= 35) {
                    creciendo = false;
                }
            } else {
                temperaturaSimulada -= 1;
                if (temperaturaSimulada <= 10) {
                    creciendo = true;
                }
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
