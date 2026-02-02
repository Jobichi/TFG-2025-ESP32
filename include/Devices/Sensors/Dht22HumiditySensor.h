#pragma once
#include <Devices/Sensors/Sensor.h>
#include <Devices/Sensors/Dht22Base.h>

struct Dht22HumidityConfig {
    unsigned long readPeriodMs = 2000UL;
    const char *friendlyName = "Humidity";
    const char *location = "room-name";
};

class Dht22HumiditySensor : public Sensor
{
public:
    Dht22HumiditySensor(
        Dht22Base &driver, 
        const Dht22HumidityConfig &cfg = {}
    );

    bool begin() override;
    void loop() override;

    bool isHealthy() const override { return driver_.healthy(); }
    const char* name() const override { return friendlyName_.c_str(); }
    const char* location() const override { return location_.c_str(); }

    void setEnabled(bool enable) override { enabled_ = enable; }
    bool isEnabled() const override { return enabled_; }

    float getValue() const override { return driver_.humidity(); }
    const char* getUnits() const override { return "%"; }

private:
    Dht22Base &driver_;
    unsigned long period_;
    unsigned long lastUpdate_{0};
    bool enabled_{true};
    String friendlyName_;
    String location_;
};
