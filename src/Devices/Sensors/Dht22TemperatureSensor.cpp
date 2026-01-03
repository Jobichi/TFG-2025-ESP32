#include <Devices/Sensors/Dht22TemperatureSensor.h>

Dht22TemperatureSensor::Dht22TemperatureSensor(
    Dht22Base &driver,
    const Dht22TemperatureConfig& cfg
)
    : driver_(driver),
      period_(cfg.readPeriodMs),
      friendlyName_(cfg.friendlyName),
      location_(cfg.location)
{}

bool Dht22TemperatureSensor::begin() {
    driver_.begin();
    lastUpdate_ = millis();
    return true;
}

void Dht22TemperatureSensor::loop() {
    if (!enabled_) return;
    if (millis() - lastUpdate_ < period_) return;

    lastUpdate_ = millis();
    driver_.update();
}
