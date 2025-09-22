#include <Arduino.h>
#include "Connectivity/WifiManager.h"
#include "Connectivity/MqttManager.h"
#include "CommandHandler.h"
#include "Actuators/Relay.h"
#include "Actuators/Buzzer.h"

// Actuadores de este ESP32
Buzzer buzzer1(18, "Buzzer1");

// Array genérico de actuadores
ActuatorBase* actuators[] = { &buzzer1 };

void setup() {
    Serial.begin(115200);

    setupWifi();
    setupMqtt();

    // Registrar los actuadores de este dispositivo
    setupCommandHandler(actuators, sizeof(actuators) / sizeof(actuators[0]));
}

void loop() {
    handleWifi();
    handleMqtt();
}
