#pragma once
#include <Devices/Actuators/Actuator.h>

struct RelayConfig{
    bool activeLow = true;
    const char *friendlyName = "Relay";
    const char *location = "room-name";
};

class Relay : public Actuator
{
public:
    explicit Relay(
        uint8_t pin, 
        const RelayConfig &cfg = {}
    );

    bool begin() override;
    void loop() override;
    const char *name() const override { return friendlyName_.c_str(); }
    const char *location() const override { return location_.c_str(); }

    bool isActive() const override { return active_; }

    String stateString() const override { return active_ ? "ON" : "OFF"; }

    // Interpretación de comandos MQTT
    bool applyCommand(const char *command) override;

private:
    uint8_t pin_;
    bool activeLow_;
    String friendlyName_;
    String location_;
    bool active_{false};
};