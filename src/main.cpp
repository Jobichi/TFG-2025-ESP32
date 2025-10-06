#include <Arduino.h>
#include "Connectivity/WifiManager.h"
#include "Connectivity/MqttManager.h"
#include "Connectivity/mqtt/announceManager.h"
#include "Connectivity/mqtt/setManager.h"
#include "Connectivity/mqtt/getManager.h"

// ==== Sensores ====
#include "Sensors/Dht22Sensor.h"
#include "Sensors/LdrSensor.h"
#include "Sensors/PirSensor.h"

// ==== Actuadores ====
#include "Actuators/Relay.h"
#include "Actuators/Buzzer.h"
#include "Actuators/ServoMotor360.h"

// -----------------------------------------------------------------------------
// CONFIGURACIÓN DE DISPOSITIVOS
// -----------------------------------------------------------------------------

// ==== Instancias de sensores ====
Dht22Sensor dht22(23, 5000, "DHT22", "salon");     // GPIO 23
LdrSensor ldr(34, 2000, "LDR", "salon");           // GPIO 34 (entrada analógica)
PirSensor pir(25, true, true, 10000, 2000, "PIR", "pasillo"); // GPIO 25

SensorBase* sensors[] = { &dht22, &ldr };
int sensorCount = sizeof(sensors) / sizeof(sensors[0]);

// ==== Instancias de actuadores ====
Relay luz(19, true, "LuzPrincipal", "salon");       // GPIO 19
Buzzer buzzer(18, "Buzzer1", "cocina");             // GPIO 18
ServoMotor360 ventana(21, 0, "Ventana", "salon");   // GPIO 21, canal PWM 0

ActuatorBase* actuators[] = { &luz, &buzzer, &ventana };
int actuatorCount = sizeof(actuators) / sizeof(actuators[0]);

// -----------------------------------------------------------------------------
// SETUP PRINCIPAL
// -----------------------------------------------------------------------------
void setup() {
    Serial.begin(115200);
    delay(500);

    // --- Inicialización de conectividad ---
    setupWifi();
    setupMqtt();

    // --- Inicialización de managers ---
    setupAnnounceManager(DEVICE_ID, sensors, sensorCount, actuators, actuatorCount);
    setupSetManager(actuators, actuatorCount);
    setupGetManager(sensors, sensorCount);

    // --- Inicialización de hardware ---
    for (int i = 0; i < sensorCount; i++) sensors[i]->begin();
    for (int i = 0; i < actuatorCount; i++) actuators[i]->begin();

    // --- Publicar announces iniciales ---
    publishAllAnnouncesOnce();

    Serial.println("[SYSTEM] Inicialización completa ✅");
}

// -----------------------------------------------------------------------------
// LOOP PRINCIPAL
// -----------------------------------------------------------------------------
void loop() {
    handleWifi();
    handleMqtt();

    // Ejecución de los loops de sensores y actuadores
    for (int i = 0; i < sensorCount; i++) sensors[i]->loop();
    for (int i = 0; i < actuatorCount; i++) actuators[i]->loop();

    // Republicar announces cada 60s
    /** 
    static unsigned long lastAnnounce = 0;
    if (millis() - lastAnnounce > 60000) {
        handleAnnounce();
        lastAnnounce = millis();
    }
    **/
}
