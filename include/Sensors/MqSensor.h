#pragma once
#include <Arduino.h>
#include "Sensors/SensorBase.h"

class MqSensor : public SensorBase{
    public:
        explicit MqSensor(uint8_t pin,
            unsigned long readPeriodMs = 1000,
            float threshold = 0.0,
            const char* friendlyName = "MQSensor");

        bool begin() override;
        void loop() override;
        bool isHealthy() override { return healthy_; }
        const char* name() const override { return friendlyName_; }

        int lastRaw() const { return lastRaw_; }
        float lastVoltage() const { return lastVoltage_; }
        unsigned long lastSampleMs() const { return lastReadMs_; }

        // Umbral de Trigger (en caso de uso)
        bool triggered() const{
            return (thresholdV_ > 0.0 && (lastVoltage_ > thresholdV_));
        }
        
        float threshold() const { return thresholdV_; }
        
    private:
        uint8_t pin_;
        unsigned long readPeriodMs_;
        const char* friendlyName_;
        float thresholdV_;

        bool healthy_{false};
        int lastRaw_{0};
        float lastVoltage_{0.0};
        unsigned long lastReadMs_{0};
};