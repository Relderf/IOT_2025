#ifndef WIFICONN_H
#define WIFICONN_H

#include <WiFiClientSecure.h>

class WifiConn {
private:
    WiFiClientSecure clienteWifi;
    int attempts = 0;
    const int maxAttempts = 15;

public:
    WifiConn();
    WiFiClientSecure& getCliente();
    void connect(String ssid, String password);
    bool isConnected();
};

#endif //WIFICONN_H
