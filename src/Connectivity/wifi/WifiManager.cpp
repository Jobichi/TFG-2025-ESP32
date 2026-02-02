#include <Connectivity/wifi/WifiManager.h>

WifiManager::WifiManager() {}

void WifiManager::begin() {
    if(Constants::DEBUG){
        Serial.println("[WiFi-Manager] Inicializando modulo WiFi...");
    }

    // Configuración de IP estática
    IPAddress ip(
        Constants::Wifi::WIFI_IP[0],
        Constants::Wifi::WIFI_IP[1],
        Constants::Wifi::WIFI_IP[2],
        Constants::Wifi::WIFI_IP[3]
    );

    // Configuración gateway
    IPAddress gateway(
        Constants::Wifi::WIFI_GATEWAY[0],
        Constants::Wifi::WIFI_GATEWAY[1],
        Constants::Wifi::WIFI_GATEWAY[2],
        Constants::Wifi::WIFI_GATEWAY[3]
    );

    // Configuración mascara de subred
    IPAddress subnet(
        Constants::Wifi::WIFI_SUBNET[0],
        Constants::Wifi::WIFI_SUBNET[1],
        Constants::Wifi::WIFI_SUBNET[2],
        Constants::Wifi::WIFI_SUBNET[3]
    );

    // Configuración de la conectividad wifi
    WiFi.config(ip, gateway, subnet);
    WiFi.mode(WIFI_STA);    // Modo del ESP32 como estación 
    // Inicialización de la conectividad
    WiFi.begin(
        Constants::Wifi::WIFI_SSID,
        Constants::Wifi::WIFI_PASS
    );
    // Log para Debuggear
    if (Constants::DEBUG) {
        Serial.printf("[WiFi-Manager] Conectando a SSID: %s\n", Constants::Wifi::WIFI_SSID);
    }
}

// Obtención del estado de la conectividad
bool WifiManager::isConnected(){
    return WiFi.status() == WL_CONNECTED;
}

// Procedimiento durante el loop
void WifiManager::loop(){
    if(!isConnected()){
        unsigned long now = millis();
        if(now - lastReconnectAttempt_ >= reconnectInterval_){
            lastReconnectAttempt_ = now;

            if(Constants::DEBUG){
                Serial.println("[WiFi-Manager] WiFi desconectado. Reintentando...");
            }

            WiFi.disconnect(true);
            WiFi.reconnect();
        }
    }
}