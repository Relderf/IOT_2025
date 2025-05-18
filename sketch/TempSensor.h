#ifndef TEMPSENSOR_H
#define TEMPSENSOR_H

#include <DHT.h>

class TempSensor {
private:
    float ultimaTemperatura;
    unsigned long ultimaLecturaTempMs;
    unsigned long intervaloLecturaTempMs;
    bool mock;
    DHT dht;

    void checkTemperatura();

public:
    TempSensor(int pin, unsigned long intervaloLecturaTempMs, bool mock = false);
    void init();
    float getTemperatura();
    bool temperaturaValida();
};

#endif //TEMPSENSOR_H
