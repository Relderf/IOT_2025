#ifndef TEMPSENSOR_H
#define TEMPSENSOR_H

#include <DHT.h>

class TempSensor {
private:
    DHT dht;

public:
    TempSensor(int pin);
    void init();
    float getTemperatura();
};

#endif //TEMPSENSOR_H
