#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Devices/Sensors/Sensor.h>

class MqSensor : public Sensor {
public:
    explicit MqSensor(
        uint8_t pin,
        unsigned long readPeriodMs = 1000,
        float thresholdVoltage = 0.0,
        const char* friendlyName = "MQSensor",
        const char* location = "room-name"
    );

    bool begin() override;
    void loop() override;

    bool isHealthy() const override { return healthy_; }

    const char* name() const override { return friendlyName_.c_str(); }
    const char* location() const override { return location_.c_str(); }

    // Activación / desactivación desde SET
    void setEnabled(bool enable) override { enabled_ = enable; }
    bool isEnabled() const override { return enabled_; }

    // Valor principal reportado → VOLTAJE
    float getValue() const override { return lastVoltage_; }
    const char* getUnits() const override { return "V"; }

    // Lecturas internas opcionales
    int lastRaw() const { return lastRaw_; }
    float lastVoltage() const { return lastVoltage_; }
    unsigned long lastSampleMs() const { return lastReadMs_; }

    // Umbral opcional para alertas
    bool triggered() const { return lastVoltage_ >= thresholdV_; }

    void simulateVoltage(float v) { lastVoltage_ = v; lastRaw_ = v * 4095.0f / 3.3f; }

private:
    uint8_t pin_;
    unsigned long readPeriodMs_;
    float thresholdV_;
    String friendlyName_;
    String location_;

    bool enabled_{true};
    bool healthy_{false};

    int lastRaw_{0};
    float lastVoltage_{0.0};
    unsigned long lastReadMs_{0};
};
