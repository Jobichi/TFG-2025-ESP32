#pragma once
#include "Sensors/SensorBase.h"
#include <Arduino.h>

class PirSensor : public SensorBase {
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

        bool isHealthy() override { return healthy_; }
        const char* name() const override { return friendlyName_.c_str(); }
        const char* location() const override { return location_.c_str(); }
        bool motion() const { return motion_; }
        unsigned long lastChangeMs() const { return lastChangeMs_; }

        std::map<String, float> readValues() override;

        String stateString() override;

    private:
        uint8_t pin_;
        bool activeHigh_;
        bool usePulldown_;
        unsigned long warmupMs_;
        unsigned long stableMs_;
        String friendlyName_;
        String location_;

        bool healthy_{false};
        bool motion_{false};
        bool lastStable_{false};
        unsigned long bootMs_{0};
        unsigned long lastChangeMs_{0};
        bool candidate_{false};
        unsigned long candidateMs_{0};
};

