#include <Devices/Sensors/WaterLeakSensor.h>

WaterLeakSensor::WaterLeakSensor(
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

bool WaterLeakSensor::begin() {
    pinMode(pin_, INPUT_PULLUP); // según módulo
    return true;
}

void WaterLeakSensor::loop() {
    if (!enabled_) return;

    bool raw = digitalRead(pin_);
    leak_ = activeHigh_ ? raw : !raw;
}
