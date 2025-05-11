#ifndef WIFICONN_H
#define WIFICONN_H

#include <WiFiClientSecure.h>

class WifiConn {
private:
    WiFiClientSecure clienteWifi;

public:
    WifiConn();
    WiFiClientSecure& getCliente();
    void connect(String ssid, String password);
};



#endif //WIFICONN_H
