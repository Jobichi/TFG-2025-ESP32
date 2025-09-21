#include <Arduino.h>
#include "Connectivity/WifiManager.h"
#include "Connectivity/MqttManager.h"

void setup() {
    Serial.begin(115200);

    setupWifi();
    setupMqtt();   // Aquí ya se manda un PING inicial
}

void loop() {
    handleWifi();
    handleMqtt();

    // 👇 Ya no mandamos pings aquí, solo mensajes de sensores/actuadores cuando toque
}
