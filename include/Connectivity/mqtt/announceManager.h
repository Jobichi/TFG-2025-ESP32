#pragma once
#include "Connectivity/MqttManager.h"

// Configura el manager de anuncios
void setupAnnounceManager(
    const char* deviceName,
    SensorBase** sensors, int sensorCount,
    ActuatorBase** actuators, int actuatorCount
);

// Maneja las publicaciones de announce
void handleAnnounceMessage(const String& topic, const String& payload);
void handleAnnounce();
void publishAllAnnouncesOnce();