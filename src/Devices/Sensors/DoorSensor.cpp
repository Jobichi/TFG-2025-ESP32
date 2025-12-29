#include <Devices/Sensors/DoorSensor.h>

DoorSensor::DoorSensor(
    uint8_t pin,
    bool activeHigh,
    const char* friendlyName,
    const char* location
)
    : pin_(pin),
      activeHigh_(activeHigh),
      friendlyName_(friendlyName),
      location_(location)
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
