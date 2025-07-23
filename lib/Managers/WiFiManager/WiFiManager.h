#pragma once

#include <ConnectionManager.h>
#include <WiFi.h>
#include <LCDscreen.h>

/* 
    Clase hija, encargada de definir y sobreescribir la clase
    ConnectionManager para la conectividad WiFi de nuestro ESP32.
*/

class WifiManager : public ConnectionManager {
    private:
        const char *_ssid;
        const char *_password;
        int _ledPin; 
        LCDScreen* _screen;  // Pantalla opcional

        void showMessage(const String& line1, const String& line2 = "", bool scroll = false);

    public:
        // Constructor para la clase WifiManager:
        WifiManager(const char *ssid, const char *password,
                    int ledPin, unsigned long time = 30000, int retries = 3,
                    LCDScreen* screen = nullptr);

        // Sobreescribimos las definiciones de la clase padre:
        void connect() override;
        void checkConnection() override;
        bool isConnected() override;
};
