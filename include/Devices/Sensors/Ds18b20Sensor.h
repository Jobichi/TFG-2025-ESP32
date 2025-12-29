#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Devices/Sensors/Sensor.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class Ds18b20Sensor : public Sensor {
public:
    explicit Ds18b20Sensor(
        uint8_t pin,
        unsigned long readPeriodMs = 2000,
        const char* friendlyName = "DS18B20",
        const char* location = "room-name"
    );

    bool begin() override;
    void loop() override;

    bool isHealthy() const override { return healthy_; }

    const char* name() const override { return friendlyName_.c_str(); }
    const char* location() const override { return location_.c_str(); }

    // Activación/desactivación (MQTT SET)
    void setEnabled(bool enable) override { enabled_ = enable; }
    bool isEnabled() const override { return enabled_; }

    // Valor principal (temperatura en ºC)
    float getValue() const override { return lastT_; }
    const char* getUnits() const override { return "°C"; }

    // Acceso opcional
    float lastTemperatureC() const { return lastT_; }

private:
    uint8_t pin_;
    unsigned long readPeriodMs_;
    String friendlyName_;
    String location_;

    OneWire oneWire_;
    DallasTemperature sensors_;

    bool enabled_{true};
    bool healthy_{false};
    float lastT_{NAN};
    unsigned long lastReadMs_{0};
};
