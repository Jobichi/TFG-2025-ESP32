#include "Logic/SensorManager.h"
#include "Connectivity/MqttManager.h"
#include <Arduino.h>

static SensorBase** g_sensors = nullptr;
static int g_sensorCount = 0;

void setupSensorManager(SensorBase** sensors, int count) {
    g_sensors = sensors,
    g_sensorCount = count;

    for (int i = 0; i < g_sensorCount; i++) {
        g_sensors[i] -> begin();

        #if DEBUG
            Serial.printf("[SENSORS] Registrado sensor: %s (index %d)\n",
                          g_sensors[i]->name(), i);
        #endif
    }
}

void handleSensors() {
    static unsigned long lastPublish = 0;
    const unsigned long publishInterval = 30000; // 30 segundos

    for (int i = 0; i < g_sensorCount; i++) {
        g_sensors[i]->loop(); // el sensor se actualiza con su propio intervalo
    }

    unsigned long now = millis();
    if (now - lastPublish >= publishInterval) {
        lastPublish = now;

        for (int i = 0; i < g_sensorCount; i++) {
            if (g_sensors[i]->isHealthy()) {
                auto values = g_sensors[i]->readValues();

                JsonDocument doc;
                
                for (auto const& kv : values) {
                    doc[kv.first] = kv.second;
                }

                String topic = String("sensors/") + DEVICE_ID + "/" + g_sensors[i]->name();
                String payload;
                serializeJson(doc, payload);

                mqttPublish(topic.c_str(), payload.c_str());

                #if DEBUG
                    Serial.printf("[SENSORS] Publicado en [%s]: %s\n",
                                  topic.c_str(), payload.c_str());
                #endif
            }
        }
    }
}

void handleSensorRequest(const String& topic, const String& payload) {
    if (topic.startsWith("get/")) {
        String sensorName = topic.substring(topic.lastIndexOf("/") + 1);

        for (int i = 0; i < g_sensorCount; i++) {
            if (sensorName == g_sensors[i]->name()) {
                auto values = g_sensors[i]->readValues();

                JsonDocument doc;

                for (auto const& kv : values) {
                    doc[kv.first] = kv.second;
                }

                String topicResp = String("sensors/") + DEVICE_ID + "/" + g_sensors[i]->name();
                String payloadResp;
                serializeJson(doc, payloadResp);

                mqttPublish(topicResp.c_str(), payloadResp.c_str());

                #if DEBUG
                    Serial.printf("[SENSORS] Respuesta a petición de [%s] → %s\n",
                                  sensorName.c_str(), payloadResp.c_str());
                #endif
            }
        }
    }
}

