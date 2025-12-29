#pragma once
#include <DHT.h>

class Dht22Base {
public:
    Dht22Base(uint8_t pin)
        : dht_(pin, DHT22)
    {}

    void begin() {
        dht_.begin();
    }

    void update() {
        lastT_ = dht_.readTemperature();
        lastH_ = dht_.readHumidity();
        healthy_ = !isnan(lastT_) && !isnan(lastH_);
        lastReadMs_ = millis();
    }

    bool healthy() const { return healthy_; }
    float temperature() const { return lastT_; }
    float humidity() const { return lastH_; }
    unsigned long lastReadTime() const { return lastReadMs_; }

private:
    DHT dht_;
    float lastT_{NAN};
    float lastH_{NAN};
    bool healthy_{false};
    unsigned long lastReadMs_{0};
};
