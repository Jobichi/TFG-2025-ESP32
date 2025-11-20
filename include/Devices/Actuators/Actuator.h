#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

class Actuator {
    public:
        virtual ~Actuator() {}

        // Ciclo de vida
        virtual bool begin() = 0;
        virtual void loop() = 0;

        // Identificación
        virtual const char* name() const = 0;
        virtual const char* location() const = 0;

        // Estado del actuador
        virtual String stateString() const = 0;
        virtual bool isActive() const = 0;

        // Comandos desde SET
        virtual bool applyCommand(const char *cmd) = 0;

        // MQTT UPDATE
        virtual void toUpdateJson(JsonDocument &doc) {
            doc["state"] = stateString();
        }

        // MQTT ANNOUNCE
        virtual void toAnnounceJson(JsonDocument &doc) {
            doc["name"] = name();
            doc["location"] = location();
        }
};
