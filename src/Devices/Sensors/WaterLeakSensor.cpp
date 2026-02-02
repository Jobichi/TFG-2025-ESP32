#include <Devices/Sensors/WaterLeakSensor.h>

WaterLeakSensor::WaterLeakSensor(
    uint8_t pin,
    const WaterLeakConfig &cfg
)
    : pin_(pin),
      activeHigh_(cfg.activeHigh),
      friendlyName_(cfg.friendlyName),
      location_(cfg.location)
{}

bool WaterLeakSensor::begin() {
    pinMode(pin_, INPUT_PULLUP); // según módulo
    return true;
}

void WaterLeakSensor::loop() {
    if (!enabled_) return;

    bool raw = digitalRead(pin_);
    leak_ = activeHigh_ ? raw : !raw;
}
