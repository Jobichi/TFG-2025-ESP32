#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Devices/Sensors/Sensor.h>

struct SoilMoistureConfig{
    unsigned long readPeriodMs = 2000UL;
    int rawDry = 3100;
    int rawWet = 2150;
    int thresholdPercent = 40;
    int hysteresisPercent = 5;
    const char *friendlyName = "SoilMoisture";
    const char *location = "room-name";
};

class SoilMoistureSensor : public Sensor {
public:
    explicit SoilMoistureSensor(
        uint8_t pin,
        const SoilMoistureConfig &cfg = {}
    );

    bool begin() override;
    void loop() override;

    bool isHealthy() const override { return healthy_; }

    const char* name() const override { return friendlyName_.c_str(); }
    const char* location() const override { return location_.c_str(); }

    // Activación / desactivación (SetHandler)
    void setEnabled(bool enable) override { enabled_ = enable; }
    bool isEnabled() const override { return enabled_; }

    // Valor principal del sensor → % humedad
    float getValue() const override { return lastPercent_; }
    const char* getUnits() const override { return "%"; }

    // Datos opcionales
    int lastRaw() const { return lastRaw_; }
    float lastVoltage() const { return lastVoltage_; }
    int moisturePercent() const { return lastPercent_; }

    bool isDry() const { return dryState_; }

    void setThreshold(int threshold) { thresholdPercent_ = threshold; }
    void setHysteresis(int hysteresis) { hysteresisPercent_ = hysteresis; }

private:
    uint8_t pin_;
    unsigned long readPeriodMs_;
    int rawDry_;
    int rawWet_;
    int thresholdPercent_;
    int hysteresisPercent_;
    String friendlyName_;
    String location_;

    bool enabled_{true};
    bool healthy_{false};

    int lastRaw_{0};
    float lastVoltage_{0.0};
    int lastPercent_{0};
    bool dryState_{false};
    unsigned long lastReadMs_{0};
};
