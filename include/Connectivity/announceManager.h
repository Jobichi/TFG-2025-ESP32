#pragma once
#include "Sensors/SensorBase.h"
#include "Actuators/ActuatorBase.h"

// Configura el manager de anuncios
void setupAnnounceManager(
    const char* deviceName,
    SensorBase** sensors, int sensorCount,
    ActuatorBase** actuators, int actuatorCount
);

// Maneja las publicaciones de announce
void handleAnnounce();
