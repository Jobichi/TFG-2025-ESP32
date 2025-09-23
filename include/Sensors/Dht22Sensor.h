#pragma once
#include "Sensors/SensorBase.h"
#include <DHT.h>

class Dht22Sensor : public SensorBase {
public:
    explicit Dht22Sensor(uint8_t pin,
                         unsigned long readPeriodMs,
                         const char* friendlyName);

    bool begin() override;
    void loop() override;
    bool isHealthy() override { return healthy_; }
    const char* name() const override { return friendlyName_; }

    float lastTemperatureC() const { return lastT_; }
    float lastHumidity() const { return lastH_; }
    unsigned long lastSampleMs() const { return lastReadMs_; }
    
    std::map<String, float> readValues() override;

private:
    uint8_t pin_;
    unsigned long readPeriodMs_;
    const char* friendlyName_;

    DHT dht_;
    bool healthy_{false};
    float lastT_{NAN};
    float lastH_{NAN};
    unsigned long lastReadMs_{0};
};
