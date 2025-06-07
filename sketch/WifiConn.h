#ifndef WIFICONN_H
#define WIFICONN_H

#include <WiFiClientSecure.h>
#include <WiFi.h>

class WifiConn {
private:
    WiFiClientSecure clienteWifi;
    WiFiClient clienteWifiInseguro;
    int attempts;
    int maxAttempts;
	unsigned long connectionDelayMs;

public:
    WifiConn(int maxAttempts = 10, unsigned long connectionDelayMs = 20000);
    WiFiClientSecure& getCliente();
    WiFiClient& getClienteInseguro();
    void connect(String ssid, String password);
    bool isConnected();
};

#endif //WIFICONN_H
