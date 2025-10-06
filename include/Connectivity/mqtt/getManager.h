#pragma once
#include "Connectivity/MqttManager.h"

// Configurar el manager GET
void setupGetManager(SensorBase** sensors, int sensorCount);

// Procesa un mensaje GET recibido desde MQTT
void handleGetMessage(const String& topic, const String& payload);