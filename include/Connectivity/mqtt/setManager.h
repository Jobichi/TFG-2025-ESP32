#pragma once
#include "Connectivity/MqttManager.h"

// Configurar el manager de SET
void setupSetManager(ActuatorBase** actuators, int actuatorCount);

// Procesa un mensaje SET recibido desde MQTT
void handleSetMessage(const String& topic, const String& paylaod);