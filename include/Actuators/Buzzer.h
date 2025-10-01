#pragma once
#include "Actuators/ActuatorBase.h"

class Buzzer : public ActuatorBase{
    public:
        explicit Buzzer(
            uint8_t pin, 
            const char* friendlyName = "Buzzer",
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
        String friendlyName_;
        String location_;
        bool active_{false};
};