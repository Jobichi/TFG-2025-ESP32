#pragma once
#include <Devices/Sensors/Sensor.h>
#include <Arduino.h>

class PirSensor : public Sensor {
public:
    explicit PirSensor(
        uint8_t pin,
        bool activeHigh = true,
        bool usePulldown = false,
        unsigned long warmupMs = 60000,
        unsigned long stableMs = 80,
        const char* friendlyName = "PIR",
        const char* location = "room-name"
    );

    bool begin() override;
    void loop() override;

    bool isHealthy() const override { return healthy_; }
    const char* name() const override { return friendlyName_.c_str(); }
    const char* location() const override { return location_.c_str(); }

    // Nueva API Sensor
    void setEnabled(bool enable) override { enabled_ = enable; }
    bool isEnabled() const override { return enabled_; }

    float getValue() const override { return motion_ ? 1.0f : 0.0f; }
    const char* getUnits() const override { return ""; }

    bool motion() const { return motion_; }
    unsigned long lastChangeMs() const { return lastChangeMs_; }

    void simulateState(bool s) { motion_ = s; }


private:
    uint8_t pin_;
    bool activeHigh_;
    bool usePulldown_;
    unsigned long warmupMs_;
    unsigned long stableMs_;
    String friendlyName_;
    String location_;

    bool enabled_{true};
    bool healthy_{false};
    bool motion_{false};
    bool lastStable_{false};
    unsigned long bootMs_{0};
    unsigned long lastChangeMs_{0};
    bool candidate_{false};
    unsigned long candidateMs_{0};
};
