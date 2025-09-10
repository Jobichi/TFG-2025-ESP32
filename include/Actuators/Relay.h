#pragma once
#include "Actuators/ActuatorBase.h"

class Relay : public ActuatorBase {
    public:
        explicit Relay(
            uint8_t pin,
            bool activeLow = true,
            const char* friendlyName = "Relay"
        );

        bool begin() override;
        void loop() override;
        const char* name() const override { return friendlyName_; }

        void on() override;
        void off() override;
        bool isActive() const override { return active_; } 
        
    private:
        uint8_t pin_;
        bool activeLow_;
        const char* friendlyName_;
        bool active_{false};
};