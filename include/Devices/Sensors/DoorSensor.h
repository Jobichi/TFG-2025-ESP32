#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Devices/Sensors/Sensor.h>

struct DoorSensorConfig{
    bool activeHigh = true; // HIGH = puerta abierta (con INPUT_PULLUP)
    const char *friendlyName = "DoorSensor";
    const char *location = "room-name";
};
class DoorSensor : public Sensor
{
public:
    explicit DoorSensor(
        uint8_t pin,
        const DoorSensorConfig &cfg = {}
    );

    bool begin() override;
    void loop() override;

    bool isHealthy() const override { return true; }

    const char* name() const override { return friendlyName_.c_str(); }
    const char* location() const override { return location_.c_str(); }

    void setEnabled(bool enable) override { enabled_ = enable; }
    bool isEnabled() const override { return enabled_; }

    // Valor binario para MQTT
    float getValue() const override { return opened_ ? 1.0f : 0.0f; }
    const char* getUnits() const override { return ""; }

    bool isOpen() const { return opened_; }

    void simulateState(bool s) { opened_ = s; }

private:
    uint8_t pin_;
    bool activeHigh_;
    String friendlyName_;
    String location_;

    bool enabled_{true};
    bool opened_{false};
};
