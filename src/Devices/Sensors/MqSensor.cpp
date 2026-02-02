#include <Devices/Sensors/MqSensor.h>

MqSensor::MqSensor(
    uint8_t pin,
    const MqConfig &cfg
)
    : pin_(pin),
      readPeriodMs_(cfg.readPeriodMs),
      thresholdV_(cfg.thresholdVoltage),
      friendlyName_(cfg.friendlyName),
      location_(cfg.location)
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
