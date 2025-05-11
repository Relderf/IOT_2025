#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H

#include <Arduino.h>

class MotorDriver {
private:
    int pin;
    bool encendido;
    int duracionEncendidoMs;
    unsigned long tiempoInicioEncendidoMs;

public:
    MotorDriver(int pin);
    void init(bool encendido, int duracionEncendidoMs);
    void encender();
    void apagar();
    bool estaEncendido();
    void loopUpdate();
};

#endif //MOTORDRIVER_H
