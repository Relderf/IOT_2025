#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H

#include <Arduino.h>
#include "config.h"

class MotorDriver {
private:
    int pin;
    bool encendido;
    int duracionEncendidoMs;
    unsigned long tiempoInicioEncendidoMs;
    bool ventanasAbiertas;

public:
    MotorDriver(int pin);
    void init(bool encendido, int duracionEncendidoMs);
    void encender();
    void apagar();
    bool estaEncendido();
    void loopUpdate();
    void abrirVentanas();
    void cerrarVentanas();
    bool puedeAbrirVentanas();
    bool getEstadoVentanas();
};

extern MotorDriver motor;

#endif //MOTORDRIVER_H
