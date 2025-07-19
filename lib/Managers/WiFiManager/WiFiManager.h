#pragma once

#include <Managers/ConnectionManager.h>
#include <WiFi.h>

/* 
    Clase hija, encargada de definir y sobreescribir la clase
    ConnectionManager para la conectividad WiFi de nuestro
    ESP32.
*/

class WifiManager : ConnectionManager{
    private:
        const char *ssid;
        const char *password;
        
        // Definimos el led para comprobar de manera visual el estado del WiFi
        int ledPin; 

    public:
        // Constructor para la clase WifiManager:
        WifiManager(const char *ssid, const char *password,
                    int ledPin, unsigned long time = 150000, int retries = 5);

        // Sobreescribimos las difiniciones de la clase padre:
        void connect() override;
        void checkConnection() override;
        void isConnected() override;
};