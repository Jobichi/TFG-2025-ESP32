#pragma once
#include "Sensors/SensorBase.h"

class LdrSensor : public SensorBase {
public:
    explicit LdrSensor(uint8_t pin,
                       const char* friendlyName = "LDR",
                       unsigned long readPeriodMs = 200);

    bool begin() override;
    void loop() override;

    bool isHealthy() override { return healthy_; }
    const char* name() const override { return friendlyName_; }

    // Última lectura (0–4095)
    int lastRaw() const { return lastRaw_; }

    // Último porcentaje (0–100%)
    int lastPercent() const { return lastPercent_; }

private:
    uint8_t pin_;
    const char* friendlyName_;
    unsigned long readPeriodMs_;

    bool healthy_{false};
    unsigned long lastReadMs_{0};
    int lastRaw_{0};
    int lastPercent_{0};
};
