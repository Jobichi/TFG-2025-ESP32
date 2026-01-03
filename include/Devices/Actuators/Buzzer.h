#pragma once
#include <Devices/Actuators/Actuator.h>

struct BuzzerConfig{
    const char *friendlyName = "Buzzer";
    const char *location = "room-name";
};
class Buzzer : public Actuator
{
public:
    explicit Buzzer(
        uint8_t pin,
        const BuzzerConfig &cfg = {}
    );

    bool begin() override;
    void loop() override;

    const char* name() const override { return friendlyName_.c_str(); }
    const char* location() const override { return location_.c_str(); }

    bool isActive() const override { return active_; }

    String stateString() const override { return active_ ? "ON" : "OFF"; }

    bool applyCommand(const char* command) override;

private:
    uint8_t pin_;
    String friendlyName_;
    String location_;
    bool active_{false};
};
