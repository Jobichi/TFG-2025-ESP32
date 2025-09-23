#include <Arduino.h>
#include "Connectivity/WifiManager.h"
#include "Connectivity/MqttManager.h"
#include "Logic/SensorManager.h"
#include "Sensors/Dht22Sensor.h"

// Sensor DHT22 en GPIO23, intervalo de muestreo 5s
Dht22Sensor dht22(23, 5000, "dht22");

// Array genérico de sensores
SensorBase* sensors[] = { &dht22 };

void setup() {
    Serial.begin(115200);

    setupWifi();
    setupMqtt();

    // Registrar sensores en el manager
    setupSensorManager(sensors, sizeof(sensors) / sizeof(sensors[0]));
}

void loop() {
    handleWifi();
    handleMqtt();
    handleSensors();   // Publicación periódica
}
