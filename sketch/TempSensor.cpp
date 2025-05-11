#include "TempSensor.h"
#include <DHT.h>

#define DHTTYPE DHT22

TempSensor::TempSensor(int p) : dht(p, DHTTYPE) {
}

void TempSensor::init() {
    dht.begin();
}

float TempSensor::getTemperatura() {
  float temp = dht.readTemperature();
  return temp;
}
