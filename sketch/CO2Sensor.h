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
    bool CO2Valida();
    float getCO2();
    void setPapasNormales(float kilos);
    void setM3(float m3);
    void setVentilacion(bool activa);
    bool getVentilacion();
};

#endif //CO2SENSOR_H
