#include <Devices/Sensors/Dht22TemperatureSensor.h>

Dht22TemperatureSensor::Dht22TemperatureSensor(
    Dht22Base &driver,
    unsigned long readPeriodMs,
    const char* friendlyName,
    const char* location
)
    : driver_(driver),
      period_(readPeriodMs),
      friendlyName_(friendlyName),
      location_(location)
{}

bool Dht22TemperatureSensor::begin() {
    driver_.begin();
    return true;
}

void Dht22TemperatureSensor::loop() {
    if (!enabled_) return;
    if (millis() - lastUpdate_ < period_) return;

    lastUpdate_ = millis();
    driver_.update();
}
