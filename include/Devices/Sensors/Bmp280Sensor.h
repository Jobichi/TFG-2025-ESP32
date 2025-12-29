#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Devices/Sensors/Sensor.h>
#include <Adafruit_BMP280.h>

class Bmp280Sensor : public Sensor {
public:
    explicit Bmp280Sensor(
        uint8_t i2cAddress = 0x76,
        unsigned long readPeriodMs = 2000,
        const char* friendlyName = "BMP280 Pressure",
        const char* location = "room-name"
    );

    bool begin() override;
    void loop() override;

    bool isHealthy() const override { return healthy_; }

    const char* name() const override { return friendlyName_.c_str(); }
    const char* location() const override { return location_.c_str(); }

    // Activación para SET
    void setEnabled(bool enable) override { enabled_ = enable; }
    bool isEnabled() const override { return enabled_; }

    // Valor principal → PRESIÓN hPa
    float getValue() const override { return lastP_; }
    const char* getUnits() const override { return "hPa"; }

    // Extras de depuración
    float lastPressure() const { return lastP_; }
    float lastTemperatureC() const { return lastT_; }

private:
    uint8_t i2cAddress_;
    unsigned long readPeriodMs_;
    String friendlyName_;
    String location_;

    Adafruit_BMP280 bmp_;

    bool enabled_{true};
    bool healthy_{false};

    float lastT_{NAN};
    float lastP_{NAN};
    unsigned long lastReadMs_{0};
};
