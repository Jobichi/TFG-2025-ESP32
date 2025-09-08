#pragma once
#include "Sensors/SensorBase.h"
#include <Adafruit_BMP280.h>

class Bmp280Sensor : public SensorBase{
    public:
        explicit Bmp280Sensor(uint8_t i2cAddress = 0x76,
            unsigned long readPeriodMs = 2000,
            const char* friendlyName = "BMP280");

        bool begin() override;
        void loop() override;
        bool isHealthy() override { return healthy_; }
        const char* name() const override { return friendlyName_; }

        float lastTemperatureC() const { return lastT_; }
        float lastPressure() const { return lastP_; }
        unsigned long lastSampleMs() const { return lastReadMs_; }

    private:
        uint8_t i2cAddress_;
        unsigned long readPeriodMs_;
        const char* friendlyName_;

        Adafruit_BMP280 bmp_;
        bool healthy_{false};
        float lastT_{NAN};
        float lastP_{NAN};
        unsigned long lastReadMs_{0};
};
