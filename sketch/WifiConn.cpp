#include "WifiConn.h"
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>

WifiConn::WifiConn(int maxAttempts, unsigned long connectionDelayMs)
    : clienteWifi(), attempts(0), maxAttempts(maxAttempts), connectionDelayMs(connectionDelayMs) {
}

WiFiClientSecure& WifiConn::getCliente() {
     return clienteWifi;
}

void WifiConn::connect(String ssid, String password) {
    attempts = 0;
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    WiFi.setSleep(false);
    while ((WiFi.status() != WL_CONNECTED) && (attempts < maxAttempts)) {
        Serial.println("Conectando WiFi...");
        delay(connectionDelayMs);
        attempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("WiFi conectado. IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("ERROR: no se pudo conectar al WiFi.");
    }
}

bool WifiConn::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}
