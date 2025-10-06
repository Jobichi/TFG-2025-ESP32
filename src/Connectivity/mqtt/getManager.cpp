#include "Connectivity/mqtt/getManager.h"

static SensorBase** g_sensors = nullptr;
static int g_sensorCount = 0;

void setupGetManager(SensorBase** sensors, int sensorCount) {
    g_sensors = sensors;
    g_sensorCount = sensorCount;
}

void handleGetMessage(const String& topic, const String& payload) {
    if (!g_sensors) return;

    int sensorId = -1;
    int lastSlash = topic.lastIndexOf('/');
    if (lastSlash != -1)
        sensorId = topic.substring(lastSlash + 1).toInt();

    if (sensorId < 0 || sensorId >= g_sensorCount) return;
    SensorBase* sensor = g_sensors[sensorId];
    if (!sensor) return;

    // --- Construir JSON de respuesta ---
    JsonDocument doc;

    doc["id"] = sensorId;
    doc["type"] = "sensor";
    doc["name"] = sensor->name();
    doc["location"] = sensor->location();
    doc["value"] = String(sensor->stateString());
    doc["timestamp"] = millis();  // o usa time(nullptr) si tienes NTP

    String json;
    serializeJson(doc, json);

    String responseTopic = String("response/") + DEVICE_ID + "/sensor/" + sensorId;
    mqttPublish(responseTopic.c_str(), json.c_str());

    #if DEBUG
    Serial.printf("[GET] Respuesta JSON publicada en %s → %s\n", responseTopic.c_str(), json.c_str());
    #endif
}