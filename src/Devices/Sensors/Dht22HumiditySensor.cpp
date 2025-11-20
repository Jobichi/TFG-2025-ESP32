#include <Devices/Sensors/Dht22HumiditySensor.h>

Dht22HumiditySensor::Dht22HumiditySensor(
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

bool Dht22HumiditySensor::begin() {
    driver_.begin();
    return true;
}

void Dht22HumiditySensor::loop() {
    if (!enabled_) return;
    if (millis() - lastUpdate_ < period_) return;

    lastUpdate_ = millis();
    driver_.update();
}
