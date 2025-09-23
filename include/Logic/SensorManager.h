#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "Sensors/SensorBase.h"

// Inicialización de los sensores del dispositivo:
void setupSensorManager(SensorBase** sensors, int count);

// Procesar lecturas y publicarlas:
void handleSensors();

// Procesar solicitudes de publicación:
void handleSensorRequest(const String& topic, const String& payload);
