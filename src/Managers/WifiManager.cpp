#include "Managers/WifiManager.h"
#include "configCredentials.h"

unsigned long lastReconnectAttempt = 0;
unsigned long forDebug = 0;

void setupWifi(){
    WiFi.mode(WIFI_STA);    // Configuración como cliente.

    //Configuración de IP estática
    IPAddress localIP(WIFI_IP);
    IPAddress gateway(WIFI_GATEWAY);
    IPAddress subnet(WIFI_SUBNET);
    WiFi.config(localIP, gateway, subnet);

    // Conecxión:
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    #if DEBUG
        Serial.print("[WiFi] Conectando");
    #endif

    unsigned long start = millis();
    while(!wifiConnected() && millis() - start < 10000){
        delay(500);
        Serial.print(".");
    }

    if(wifiConnected()){
        Serial.print("\n[WiFi] conectado. IP: ");
        Serial.println(WiFi.localIP());
    } else {
        #if DEBUG
            Serial.println("\n[WiFi] No se pudo conectar en setup. Reintentos en loop.");
        #endif
        Serial.println("\n[WiFi] No se pudo conectar en setup. Reintentos en loop.");
    }

    
}

bool wifiConnected(){
    return WiFi.status() == WL_CONNECTED;
}

void handleWifi() {
    unsigned long now = millis();

    if (!wifiConnected()) {
        if (now - lastReconnectAttempt > 5000) {  // cada 5s
            lastReconnectAttempt = now;
            #if DEBUG
                Serial.println("[WiFi] Intentando reconectar...");
            #endif
            WiFi.disconnect();
            WiFi.begin(WIFI_SSID, WIFI_PASS);
        } else {
            #if DEBUG
                Serial.printf("[WiFi] No reconectado todavía. Tiempo: %lu\n", now);
            #endif
        }
    } else if(now - forDebug > 60000){
        forDebug = now;
        #if DEBUG
            Serial.println("[WiFi] OK.");
        #endif
    }
}