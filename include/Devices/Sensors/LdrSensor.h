#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Devices/Sensors/Sensor.h>

struct LdrConfig
{
    unsigned long readPeriodMs = 200UL;
    const char *friendlyName = "LDR";
    const char *location = "room-name";
};

class LdrSensor : public Sensor {
public:
    explicit LdrSensor(
        uint8_t pin,
        const LdrConfig &cfg = {}
    );

    bool begin() override;
    void loop() override;

    bool isHealthy() const override { return healthy_; }

    const char* name() const override { return friendlyName_.c_str(); }
    const char* location() const override { return location_.c_str(); }

    // Activación / desactivación (MQTT SET)
    void setEnabled(bool enable) override { enabled_ = enable; }
    bool isEnabled() const override { return enabled_; }

    // Valor principal del sensor (luminosidad en %)
    float getValue() const override { return lastPercent_; }
    const char* getUnits() const override { return "%"; }

    // Extras opcionales
    int lastRaw() const { return lastRaw_; }
    int lastPercent() const { return lastPercent_; }

    void simulateValue(int percent) { lastPercent_ = percent; }


private:
    uint8_t pin_;
    String friendlyName_;
    String location_;
    unsigned long readPeriodMs_;

    bool enabled_{true};
    bool healthy_{false};

    unsigned long lastReadMs_{0};
    int lastRaw_{0};
    int lastPercent_{0};
};
