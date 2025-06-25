#ifndef CO2SENSOR_H
#define CO2SENSOR_H

class CO2Sensor {
private:
    float ultimoCO2;
    unsigned long ultimaLecturaCO2Ms;
    unsigned long intervaloLecturaCO2Ms;

    void checkCO2();

public:
    CO2Sensor(int pin, unsigned long intervaloLecturaCO2Ms);
    void init();
    float getCO2();
    bool CO2Valido();
};

#endif
