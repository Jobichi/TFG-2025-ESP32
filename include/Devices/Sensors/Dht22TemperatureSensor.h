#pragma once
#include <Devices/Sensors/Sensor.h>
#include <Devices/Sensors/Dht22Base.h>

struct Dht22TemperatureConfig{
    unsigned long readPeriodMs = 2000UL;
    const char *friendlyName = "Temp";
    const char *location = "room-name";
};

class Dht22TemperatureSensor : public Sensor {
public:
    Dht22TemperatureSensor(
        Dht22Base &driver,
        const Dht22TemperatureConfig &cfg = {}
    );

    bool begin() override;
    void loop() override;

    bool isHealthy() const override { return driver_.healthy(); }
    const char* name() const override { return friendlyName_.c_str(); }
    const char* location() const override { return location_.c_str(); }

    void setEnabled(bool enable) override { enabled_ = enable; }
    bool isEnabled() const override { return enabled_; }

    float getValue() const override { return driver_.temperature(); }
    const char* getUnits() const override { return "°C"; }

private:
    Dht22Base &driver_;
    unsigned long period_;
    unsigned long lastUpdate_{0};
    bool enabled_{true};
    String friendlyName_;
    String location_;
};
