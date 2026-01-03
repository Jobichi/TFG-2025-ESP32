#include <Devices/Sensors/Dht22HumiditySensor.h>

Dht22HumiditySensor::Dht22HumiditySensor(
    Dht22Base &driver,
    const Dht22HumidityConfig& cfg
)
    : driver_(driver),
      period_(cfg.readPeriodMs),
      friendlyName_(cfg.friendlyName),
      location_(cfg.location)
{}

bool Dht22HumiditySensor::begin() {
    driver_.begin();
    lastUpdate_ = millis();
    return true;
}

void Dht22HumiditySensor::loop() {
    if (!enabled_) return;
    if (millis() - lastUpdate_ < period_) return;

    lastUpdate_ = millis();
    driver_.update();
}
