#include <ArduinoJson.h>
#include "Logic/actuatorController.h"

static ActuatorBase** g_actuators = nullptr;
static int g_count = 0;

void setupCommandHandler(ActuatorBase** actuators, int count) {
    g_actuators = actuators;
    g_count = count;

    for (int i = 0; i < g_count; i++) {
        g_actuators[i]->begin();
        #if DEBUG
            Serial.printf("[CMD] Registrado actuador: %s (index %d)\n",
                          g_actuators[i]->name(), i);
        #endif
    }
}

void handleMqttCommand(const String& topic, const String& payload) {
    #if DEBUG
        Serial.printf("[CMD] Procesando comando en [%s]: %s\n",
                      topic.c_str(), payload.c_str());
    #endif

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
        #if DEBUG
            Serial.printf("[CMD] Error parseando JSON: %s\n", error.c_str());
        #endif
        return;
    }

    int id = doc["id"] | -1;               // valor por defecto -1 si no existe
    const char* state = doc["state"];      // puede ser nullptr

    if (!state) {
        #if DEBUG
            Serial.println("[CMD] Campo 'state' ausente o inválido");
        #endif
        return;
    }

    if (id >= 0 && id < g_count) {
        if (strcmp(state, "ON") == 0) {
            g_actuators[id]->on();
        } else if (strcmp(state, "OFF") == 0) {
            g_actuators[id]->off();
        }

        #if DEBUG
            Serial.printf("[CMD] Actuador %d (%s) → %s\n",
                          id, g_actuators[id]->name(), state);
        #endif
    } else {
        #if DEBUG
            Serial.printf("[CMD] ID fuera de rango: %d\n", id);
        #endif
    }
}
