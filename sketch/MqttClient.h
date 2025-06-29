#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <WiFi.h>
#include <PubSubClient.h>

class MqttClient {
private:
    PubSubClient pubSubClient;
    void publicarEstadoVentilacion();
public:
    MqttClient(WiFiClient& clienteWifi);
    void init(const char* mqttAddress, uint16_t mqttPort);
    bool isConnected();
    void connect(const char* clientName, int connDelay);
    void loop();
    void publish(const char* topic, const char* payload);
    void callback(char* topic, byte* payload, unsigned int length);
    void publicarCo2(float co2);

};

#endif //MQTTCLIENT_H
