#pragma once
#include "Sensors/SensorBase.h"

class LdrSensor : public SensorBase {
public:
    explicit LdrSensor(
        uint8_t pin,
        unsigned long readPeriodMs = 200,
        const char* friendlyName = "LDR",
        const char* location = "room-name"
    );

    bool begin() override;
    void loop() override;

    bool isHealthy() override { return healthy_; }
    const char* name() const override { return friendlyName_.c_str(); }
    const char* location() const override { return location_.c_str(); }

    // Última lectura (0–4095)
    int lastRaw() const { return lastRaw_; }

    // Último porcentaje (0–100%)
    int lastPercent() const { return lastPercent_; }

    std::map<String, float> readValues() override;
    
    String stateString() override;

private:
    uint8_t pin_;
    String friendlyName_;
    String location_;
    unsigned long readPeriodMs_;

    bool healthy_{false};
    unsigned long lastReadMs_{0};
    int lastRaw_{0};
    int lastPercent_{0};
};
