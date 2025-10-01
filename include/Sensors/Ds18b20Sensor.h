#pragma once
#include "Sensors/SensorBase.h"
#include <OneWire.h>
#include <DallasTemperature.h>

class Ds18b20Sensor : public SensorBase {
public:
    explicit Ds18b20Sensor(
        uint8_t pin,
        unsigned long readPeriodMs,
        const char* friendlyName = "DS18B20",
        const char* location = "room-name"
    );

    bool begin() override;
    void loop() override;
    bool isHealthy() override { return healthy_; }
    const char* name() const override { return friendlyName_.c_str(); }
    const char* location() const override { return location_.c_str(); }

    float lastTemperatureC() const { return lastT_; }
    unsigned long lastSampleMs() const { return lastReadMs_; }

    std::map<String, float> readValues() override;

    String stateString() override;

private:
    uint8_t pin_;
    unsigned long readPeriodMs_;
    String friendlyName_;
    String location_;

    OneWire oneWire_;
    DallasTemperature sensors_;
    bool healthy_{false};
    float lastT_{NAN};
    unsigned long lastReadMs_{0};
};
