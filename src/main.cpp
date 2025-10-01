#include <Arduino.h>
#include "Connectivity/WifiManager.h"
#include "Connectivity/MqttManager.h"
#include "Logic/SensorManager.h"
#include "Logic/CommandHandler.h"
#include "Connectivity/announceManager.h"

// ==== Sensores ====
#include "Sensors/Dht22Sensor.h"
#include "Sensors/LdrSensor.h"
#include "Sensors/PirSensor.h"

// ==== Actuadores ====
#include "Actuators/Relay.h"
#include "Actuators/Buzzer.h"
#include "Actuators/ServoMotor360.h"

unsigned long lastAnnounce = 0;
bool announced = false;

// ==== Instancias de sensores ====
Dht22Sensor dht22(23, 5000, "DHT22", "salon");    // GPIO 23
LdrSensor ldr(34, 2000, "LDR", "salon");          // GPIO 34 (entrada analógica)
PirSensor pir(25, true, true, 10000, 2000, "PIR", "pasillo"); // GPIO 25

SensorBase* sensors[] = { &dht22, &ldr };
int sensorCount = sizeof(sensors) / sizeof(sensors[0]);

// ==== Instancias de actuadores ====
Relay luz(19, true, "LuzPrincipal", "salon");      // GPIO 19
Buzzer buzzer(18, "Buzzer1", "cocina");            // GPIO 18
ServoMotor360 ventana(21, 0, "Ventana", "salon");  // GPIO 21, canal PWM 0

ActuatorBase* actuators[] = { &luz, &buzzer, &ventana };
int actuatorCount = sizeof(actuators) / sizeof(actuators[0]);

// ==== Setup principal ====
void setup() {
    Serial.begin(115200);
    setupWifi();
    setupMqtt();

    setupSensorManager(sensors, sensorCount);
    setupCommandHandler(actuators, actuatorCount);

    setupAnnounceManager(DEVICE_ID, sensors, sensorCount, actuators, actuatorCount);

    // Primera publicación al arrancar
    handleAnnounce();
}

void loop() {
    handleWifi();
    handleMqtt();
    handleSensors();

    // Ejemplo: republicar cada minuto
    static unsigned long lastAnnounce = 0;
    if (millis() - lastAnnounce > 60000) {
        handleAnnounce();
        lastAnnounce = millis();
    }
}
