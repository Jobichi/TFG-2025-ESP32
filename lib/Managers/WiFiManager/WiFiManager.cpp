#pragma once

#include <Arduino.h>
#include <LCDscreen.h>
#include <WiFiManager/WiFiManager.h>

WifiManager::WifiManager(const char *ssid, const char *password, int ledPin,
                         unsigned long time, int retries)
    : ConnectionManager(time, retries), ssid(ssid), password(password),
    ledPin(ledPin) {
    
        pinMode(ledPin, OUTPUT);
        digitalWrite(ledPin, LOW);
    }

// Conexión a la WiFi:

void WifiManager::connect(){
    Serial.print("Conectando al WiFi");
}
