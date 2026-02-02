#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Devices/Sensors/Sensor.h>

struct FireConfig{
    bool activeHigh = false; // KY-026 suele ser active-LOW
    const char *friendlyName = "FireSensor";
    const char *location = "room-name";
};
class FireSensor : public Sensor
{
public:
    explicit FireSensor(
        uint8_t pin,
        const FireConfig &cfg = {}
    );

    bool begin() override;
    void loop() override;

    bool isHealthy() const override { return true; }

    const char* name() const override { return friendlyName_.c_str(); }
    const char* location() const override { return location_.c_str(); }

    void setEnabled(bool enable) override { enabled_ = enable; }
    bool isEnabled() const override { return enabled_; }

    float getValue() const override { return fireDetected_ ? 1.0f : 0.0f; }
    const char* getUnits() const override { return ""; }

    bool fireDetected() const { return fireDetected_; }

    void simulateState(bool s) { fireDetected_ = s; }


private:
    uint8_t pin_;
    bool activeHigh_;
    String friendlyName_;
    String location_;

    bool enabled_{true};
    bool fireDetected_{false};
};
