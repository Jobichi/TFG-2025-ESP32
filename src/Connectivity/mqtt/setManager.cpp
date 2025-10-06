#include "Connectivity/mqtt/setManager.h"

static ActuatorBase** g_actuators = nullptr;
static int g_actuatorCount = 0;

void setupSetManager(ActuatorBase** actuators, int actuatorCount) {
    g_actuators = actuators;
    g_actuatorCount = actuatorCount;
}

void handleSetMessage(const String& topic, const String& payload) {
    if (!g_actuators) return;

    int actuatorId = -1;
    int lastSlash = topic.lastIndexOf('/');
    if (lastSlash != -1)
        actuatorId = topic.substring(lastSlash + 1).toInt();

    if (actuatorId < 0 || actuatorId >= g_actuatorCount) return;
    ActuatorBase* actuator = g_actuators[actuatorId];
    if (!actuator) return;

    bool success = actuator->applyCommand(payload.c_str());

    // --- Construir JSON de respuesta ---
    StaticJsonDocument<256> doc;
    doc["id"] = actuatorId;
    doc["type"] = "actuator";
    doc["name"] = actuator->name();
    doc["location"] = actuator->location();
    doc["state"] = actuator->stateString();
    doc["result"] = success ? "OK" : "ERROR";
    doc["timestamp"] = millis();

    String json;
    serializeJson(doc, json);

    String responseTopic = String("response/") + DEVICE_ID + "/actuator/" + actuatorId;
    mqttPublish(responseTopic.c_str(), json.c_str());

    #if DEBUG
    Serial.printf("[SET] Respuesta JSON publicada en %s → %s\n", responseTopic.c_str(), json.c_str());
    #endif
}