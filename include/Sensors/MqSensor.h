#pragma once
#include <Arduino.h>
#include "Sensors/SensorBase.h"

class MqSensor : public SensorBase{
    public:
        explicit MqSensor(
            uint8_t pin,
            unsigned long readPeriodMs = 1000,
            float threshold = 0.0,
            const char* friendlyName = "MQSensor",
            const char* location = "room-name"
        );

        bool begin() override;
        void loop() override;
        bool isHealthy() override { return healthy_; }
        const char* name() const override { return friendlyName_.c_str(); }
        const char* location() const override { return location_.c_str(); }

        int lastRaw() const { return lastRaw_; }
        float lastVoltage() const { return lastVoltage_; }
        unsigned long lastSampleMs() const { return lastReadMs_; }

        // Umbral de Trigger (en caso de uso)
        bool triggered() const;
        
        float threshold() const { return thresholdV_; }

        std::map<String, float> readValues() override;

        String stateString() override;
        
    private:
        uint8_t pin_;
        unsigned long readPeriodMs_;
        String friendlyName_;
        String location_;
        float thresholdV_;

        bool healthy_{false};
        int lastRaw_{0};
        float lastVoltage_{0.0};
        unsigned long lastReadMs_{0};
};