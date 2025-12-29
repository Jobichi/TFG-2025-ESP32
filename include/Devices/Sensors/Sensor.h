#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

class Sensor {
public:
    virtual ~Sensor() {}

    // Ciclo de vida
    virtual bool begin() = 0;
    virtual void loop() = 0;

    // Estado de salud
    virtual bool isHealthy() const = 0;

    // Identificación
    virtual const char* name() const = 0;
    virtual const char* location() const = 0;

    // Encendido/apagado del sensor
    virtual void setEnabled(bool enable) = 0;
    virtual bool isEnabled() const = 0;

    // Medición principal
    virtual float getValue() const = 0;
    virtual const char* getUnits() const = 0;

    // MQTT UPDATE
    virtual void toUpdateJson(JsonDocument &doc) {
        doc["value"] = getValue();
        doc["units"] = getUnits();
    }

    // MQTT ANNOUNCE
    virtual void toAnnounceJson(JsonDocument &doc) {
        doc["name"] = name();
        doc["location"] = location();
    }
};
