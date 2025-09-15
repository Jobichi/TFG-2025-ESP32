#pragma once
#include "Sensors/SensorBase.h"
#include <OneWire.h>
#include <DallasTemperature.h>

class Ds18b20Sensor : public SensorBase {
public:
    explicit Ds18b20Sensor(uint8_t pin,
                           unsigned long readPeriodMs,
                           const char* friendlyName);

    bool begin() override;
    void loop() override;
    bool isHealthy() override { return healthy_; }
    const char* name() const override { return friendlyName_; }

    float lastTemperatureC() const { return lastT_; }
    unsigned long lastSampleMs() const { return lastReadMs_; }

private:
    uint8_t pin_;
    unsigned long readPeriodMs_;
    const char* friendlyName_;

    OneWire oneWire_;
    DallasTemperature sensors_;
    bool healthy_{false};
    float lastT_{NAN};
    unsigned long lastReadMs_{0};
};
