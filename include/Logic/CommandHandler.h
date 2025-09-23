#pragma once
#include <Arduino.h>
#include "Actuators/ActuatorBase.h"

// Inicialización del manejador de los actuadores:
void setupCommandHandler(ActuatorBase** actuators, int count);

// Procesamiento de comandos vía MQTT
void handleMqttCommand(const String& topic, const String& payload);