#include <Arduino.h>
#include "Connectivity/WifiManager.h"
#include "Connectivity/MqttManager.h"

void setup() {
    Serial.begin(115200);

    // Conexión WiFi
    setupWifi();

    // Conexión MQTT (se suscribe a DEVICE_ID/ping automáticamente)
    setupMqtt();
}

void loop() {
    // Mantener la conexión WiFi y MQTT
    handleWifi();
    handleMqtt();

    // Aquí iría tu lógica de sensores/actuadores
    // Ejemplo: publicar cada 5s un mensaje de estado
    static unsigned long lastMsg = 0;
    if (millis() - lastMsg > 5000) {
        lastMsg = millis();
        mqttPublish("status/" DEVICE_ID, "alive");
    }
}
