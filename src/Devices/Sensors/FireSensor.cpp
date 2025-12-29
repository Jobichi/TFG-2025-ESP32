#include <Devices/Sensors/FireSensor.h>

FireSensor::FireSensor(
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

bool FireSensor::begin() {
    pinMode(pin_, INPUT);
    return true;
}

void FireSensor::loop() {
    if (!enabled_) return;

    bool raw = digitalRead(pin_);
    fireDetected_ = activeHigh_ ? raw : !raw;
}
