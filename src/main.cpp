
#include <Arduino.h>
#include "configCredentials.h"
#include "Managers/WifiManager.h"

unsigned long lastInfoAttempt = 0;

void setup(){
    Serial.begin(115200);
    setupWifi();
}

void loop(){
    handleWifi();
}