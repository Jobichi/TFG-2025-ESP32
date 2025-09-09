#pragma once
#include "Sensors/SensorBase.h"
#include <Arduino.h>

class SoilMoistureSensor : public SensorBase {
    public:
        explicit SoilMoistureSensor(uint8_t pin,
            unsigned long readPeriodMs = 2000,
            int rawDry = 3100,
            int rawWet = 2150,
            int thresholdPercent = 40,
            int hysteresisPercent = 5,
            const char* friendlyName = "SoilMoisture");

        bool begin() override;
        void loop() override;
        bool isHealthy() override { return healthy_; }
        const char* name() const override { return friendlyName_; }

        int lastRaw() const { return lastRaw_; }
        float lastVoltage() const { return lastVoltage_; }
        int moisturePercent() const { return lastPercent_; }

        bool isDry() const { return dryState_; }

        // Métodos setter para umbral e histeresis
        void setThreshold(int threshold) { thresholdPercent_ = threshold; }
        void setHysteresis(int hysteresis) { hysteresisPercent_ = hysteresis; }

    private:
        uint8_t pin_;
        unsigned long readPeriodMs_;
        int rawDry_;
        int rawWet_;
        int thresholdPercent_;
        int hysteresisPercent_;
        const char* friendlyName_;

        bool healthy_{false};
        int lastRaw_{0};
        float lastVoltage_{0.0};
        int lastPercent_{0};
        bool dryState_{false};
        unsigned long lastReadMs_{0};
};