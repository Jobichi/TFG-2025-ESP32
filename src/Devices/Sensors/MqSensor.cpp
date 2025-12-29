#include <Devices/Sensors/MqSensor.h>

MqSensor::MqSensor(
    uint8_t pin,
    unsigned long readPeriodMs,
    float thresholdVoltage,
    const char* friendlyName,
    const char* location
)
    : pin_(pin),
      readPeriodMs_(readPeriodMs),
      thresholdV_(thresholdVoltage),
      friendlyName_(friendlyName),
      location_(location)
{}

bool MqSensor::begin() {
    pinMode(pin_, INPUT);
    healthy_ = true;
    return true;
}

void MqSensor::loop() {
    if (!enabled_) return;

    if (millis() - lastReadMs_ < readPeriodMs_)
        return;

    lastReadMs_ = millis();

    lastRaw_ = analogRead(pin_);
    lastVoltage_ = (lastRaw_ / 4095.0f) * 3.3f;

    healthy_ = !isnan(lastVoltage_);
}
