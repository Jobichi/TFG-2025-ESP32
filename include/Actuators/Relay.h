#pragma once
#include "Actuators/ActuatorBase.h"

class Relay : public ActuatorBase {
    public:
        explicit Relay(
            uint8_t pin,
            bool activeLow = true,
            const char* friendlyName = "Relay",
            const char* location = "room-name"
        );

        bool begin() override;
        void loop() override;
        const char* name() const override { return friendlyName_.c_str(); }
        const char* location() const override { return location_.c_str(); }

        void on() override;
        void off() override;
        bool isActive() const override { return active_; } 

        String stateString() override { return active_ ? "ON" : "OFF"; }
        
    private:
        uint8_t pin_;
        bool activeLow_;
        String friendlyName_;
        String location_;
        bool active_{false};
};