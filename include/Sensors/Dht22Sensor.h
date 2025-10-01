#pragma once
#include "Sensors/SensorBase.h"
#include <DHT.h>

class Dht22Sensor : public SensorBase {
public:
    explicit Dht22Sensor(
        uint8_t pin,
        unsigned long readPeriodMs,
        const char* friendlyName,
        const char* location
    );

    bool begin() override;
    void loop() override;
    bool isHealthy() override { return healthy_; }
    const char* name() const override { return friendlyName_.c_str(); }
    const char* location() const override { return location_.c_str(); }

    float lastTemperatureC() const { return lastT_; }
    float lastHumidity() const { return lastH_; }
    unsigned long lastSampleMs() const { return lastReadMs_; }
    
    std::map<String, float> readValues() override;

    String stateString() override;

private:
    uint8_t pin_;
    unsigned long readPeriodMs_;
    String friendlyName_;
    String location_;

    DHT dht_;
    bool healthy_{false};
    float lastT_{NAN};
    float lastH_{NAN};
    unsigned long lastReadMs_{0};
};
