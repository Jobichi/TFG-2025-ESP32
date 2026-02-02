#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Devices/Sensors/Sensor.h>

struct WaterLeakConfig{
    bool activeHigh = true;
    const char *friendlyName = "WaterLeak";
    const char *location = "room-name";
};
class WaterLeakSensor : public Sensor
{
public:
    explicit WaterLeakSensor(
        uint8_t pin,
        const WaterLeakConfig &cfg = {}
    );

    bool begin() override;
    void loop() override;

    bool isHealthy() const override { return true; }

    const char* name() const override { return friendlyName_.c_str(); }
    const char* location() const override { return location_.c_str(); }

    void setEnabled(bool enable) override { enabled_ = enable; }
    bool isEnabled() const override { return enabled_; }

    float getValue() const override { return leak_ ? 1.0f : 0.0f; }
    const char* getUnits() const override { return ""; }

    bool leakDetected() const { return leak_; }

    void simulateState(bool s) { leak_ = s; }


private:
    uint8_t pin_;
    bool activeHigh_;
    String friendlyName_;
    String location_;

    bool enabled_{true};
    bool leak_{false};
};
