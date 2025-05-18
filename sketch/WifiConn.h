#ifndef WIFICONN_H
#define WIFICONN_H

#include <WiFiClientSecure.h>

class WifiConn {
private:
    WiFiClientSecure clienteWifi;
    int attempts;
    int maxAttempts;
	unsigned long connectionDelayMs;

public:
    WifiConn(int maxAttempts = 5, unsigned long connectionDelayMs = 1000);
    WiFiClientSecure& getCliente();
    void connect(String ssid, String password);
    bool isConnected();
};

#endif //WIFICONN_H
