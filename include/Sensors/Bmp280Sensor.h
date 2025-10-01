#pragma once
#include "Sensors/SensorBase.h"
#include <Adafruit_BMP280.h>

class Bmp280Sensor : public SensorBase{
    public:
        explicit Bmp280Sensor(
            uint8_t i2cAddress = 0x76,
            unsigned long readPeriodMs = 2000,
            const char* friendlyName = "BMP280",
            const char* location = "room-name"
        );

        bool begin() override;
        void loop() override;
        bool isHealthy() override { return healthy_; }
        const char* name() const override { return friendlyName_.c_str(); }
        const char* location() const override { return location_.c_str(); }

        float lastTemperatureC() const { return lastT_; }
        float lastPressure() const { return lastP_; }
        unsigned long lastSampleMs() const { return lastReadMs_; }

        std::map<String, float> readValues() override;

        String stateString() override;

    private:
        uint8_t i2cAddress_;
        unsigned long readPeriodMs_;
        String friendlyName_;
        String location_;

        Adafruit_BMP280 bmp_;
        bool healthy_{false};
        float lastT_{NAN};
        float lastP_{NAN};
        unsigned long lastReadMs_{0};
};
