#include <Devices/Sensors/DoorSensor.h>

DoorSensor::DoorSensor(
    uint8_t pin,
    const DoorSensorConfig &cfg
)
    : pin_(pin),
      activeHigh_(cfg.activeHigh),
      friendlyName_(cfg.friendlyName),
      location_(cfg.location)
{}

bool DoorSensor::begin() {
    pinMode(pin_, INPUT_PULLUP);
    return true;
}

void DoorSensor::loop() {
    if (!enabled_) return;

    bool raw = digitalRead(pin_);
    opened_ = activeHigh_ ? raw : !raw;
}
