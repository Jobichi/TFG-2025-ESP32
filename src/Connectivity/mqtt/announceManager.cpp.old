#include "Connectivity/mqtt/announceManager.h"

static const char* g_deviceName = nullptr;
static SensorBase** g_sensors = nullptr;
static int g_sensorCount = 0;
static ActuatorBase** g_actuators = nullptr;
static int g_actuatorCount = 0;

static unsigned long g_lastAnnounce = 0;
static const unsigned long ANNOUNCE_INTERVAL = 60000; // 60s

void setupAnnounceManager(
    const char* deviceName,
    SensorBase** sensors, int sensorCount,
    ActuatorBase** actuators, int actuatorCount
) {
    g_deviceName = deviceName;
    g_sensors = sensors;
    g_sensorCount = sensorCount;
    g_actuators = actuators;
    g_actuatorCount = actuatorCount;

    #if DEBUG
    Serial.printf("[ANNOUNCE] Configurado para %s con %d sensores y %d actuadores\n",
                  g_deviceName, g_sensorCount, g_actuatorCount);
    #endif
}

void publishAllAnnouncesOnce() {
    if (!g_deviceName) return;

    // --- Sensores ---
    for (int i = 0; i < g_sensorCount; i++) {
        if (!g_sensors[i]) continue;
        JsonDocument doc;

        doc["id"] = i;
        doc["name"] = g_sensors[i]->name();
        doc["location"] = g_sensors[i]->location();
        doc["state"] = g_sensors[i]->stateString();

        String payload;
        serializeJson(doc, payload);

        String topic = String("announce/") + g_deviceName + "/sensor/" + i;
        mqttPublish(topic.c_str(), payload.c_str(), true);
    }

    // --- Actuadores ---
    for (int i = 0; i < g_actuatorCount; i++) {
        if (!g_actuators[i]) continue;
        JsonDocument doc;

        doc["id"] = i;
        doc["name"] = g_actuators[i]->name();
        doc["location"] = g_actuators[i]->location();
        doc["state"] = g_actuators[i]->stateString();

        String payload;
        serializeJson(doc, payload);

        String topic = String("announce/") + g_deviceName + "/actuator/" + i;
        mqttPublish(topic.c_str(), payload.c_str(), true);
    }
}

void handleAnnounce() {
    if (millis() - g_lastAnnounce >= ANNOUNCE_INTERVAL) {
        g_lastAnnounce = millis();
        publishAllAnnouncesOnce();
    }
}

void handleAnnounceMessage(const String& topic, const String& payload) {
    #if DEBUG
    Serial.printf("[ANNOUNCE] Mensaje recibido (%s): %s\n", topic.c_str(), payload.c_str());
    #endif
}
