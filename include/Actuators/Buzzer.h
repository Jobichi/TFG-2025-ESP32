#pragma once
#include "Actuators/ActuatorBase.h"

class Buzzer : public ActuatorBase{
    public:
        explicit Buzzer(uint8_t pin, const char* friendlyName = "Buzzer");

        bool begin() override;
        void loop() override;
        const char* name() const override { return friendlyName_; }
        
        void on() override;
        void off() override;
        bool isActive() const override { return active_; }

    private:
        uint8_t pin_;
        const char* friendlyName_;
        bool active_{false};
};