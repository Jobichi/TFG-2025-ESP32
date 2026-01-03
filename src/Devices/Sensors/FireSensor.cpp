#include <Devices/Sensors/FireSensor.h>

FireSensor::FireSensor(
    uint8_t pin,
    const FireConfig &cfg
)
    : pin_(pin),
      activeHigh_(cfg.activeHigh),
      friendlyName_(cfg.friendlyName),
      location_(cfg.location)
{}

bool FireSensor::begin() {
    pinMode(pin_, INPUT);
    return true;
}

void FireSensor::loop() {
    if (!enabled_) return;

    bool raw = digitalRead(pin_);
    fireDetected_ = activeHigh_ ? raw : !raw;
}
