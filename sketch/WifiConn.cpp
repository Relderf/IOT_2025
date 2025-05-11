#include "WifiConn.h"
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>

WifiConn::WifiConn() : clienteWifi() {
}

WiFiClientSecure& WifiConn::getCliente() {
     return clienteWifi;
}

void WifiConn::connect(String ssid, String password) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    WiFi.setSleep(false);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando WiFi...");
    }
    Serial.print("WiFi conectado. Dir. IP: ");
    Serial.println(WiFi.localIP());
}