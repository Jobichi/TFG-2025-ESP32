#pragma once

#include <vector>
#include <WiFi.h>
#include <Arduino.h>

class WifiScanner{
    public:
        // Se define la estructura para el contenido del vector.
        struct Network
        {
            String ssid;
            int32_t rssi;
            bool encrypted;
        };

        // Método para escanear las redes y devolver el vector.
        static std::vector<Network> scanNetworks();

        // Método para imprimir los resultados por Serial.
        static void printNetworks();
};