#include <Devices/Sensors/Ds18b20Sensor.h>

Ds18b20Sensor::Ds18b20Sensor(
    uint8_t pin,
    const Ds18b20Config &cfg
)
    : pin_(pin),
      readPeriodMs_(cfg.readPeriodMs),
      friendlyName_(cfg.friendlyName),
      location_(cfg.location),
      oneWire_(pin),
      sensors_(&oneWire_)
{}

bool Ds18b20Sensor::begin() {
    sensors_.begin();
    sensors_.setResolution(12);

    sensors_.requestTemperatures();
    lastT_ = sensors_.getTempCByIndex(0);

    healthy_ = !isnan(lastT_);
    lastReadMs_ = millis();

    return healthy_;
}

void Ds18b20Sensor::loop() {
    if (!enabled_) return;
    if (!healthy_) return;

    if (millis() - lastReadMs_ < readPeriodMs_)
        return;

    lastReadMs_ = millis();

    sensors_.requestTemperatures();
    lastT_ = sensors_.getTempCByIndex(0);

    healthy_ = !isnan(lastT_);
}
