#include <Devices/Sensors/LdrSensor.h>

LdrSensor::LdrSensor(
    uint8_t pin,
    unsigned long readPeriodMs,
    const char* friendlyName,
    const char* location
)
    : pin_(pin),
      readPeriodMs_(readPeriodMs),
      friendlyName_(friendlyName),
      location_(location)
{}

bool LdrSensor::begin() {
    pinMode(pin_, INPUT);
    healthy_ = true;
    lastReadMs_ = millis();
    return true;
}

void LdrSensor::loop() {
    if (!enabled_) return;
    if (!healthy_) return;

    if (millis() - lastReadMs_ < readPeriodMs_)
        return;

    lastReadMs_ = millis();

    lastRaw_ = analogRead(pin_);
    lastPercent_ = map(lastRaw_, 0, 4095, 0, 100);

    healthy_ = !isnan(lastPercent_);
}
