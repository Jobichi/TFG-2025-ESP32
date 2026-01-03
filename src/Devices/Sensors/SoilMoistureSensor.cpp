#include <Devices/Sensors/SoilMoistureSensor.h>

SoilMoistureSensor::SoilMoistureSensor(
    uint8_t pin,
    const SoilMoistureConfig &cfg
)
    : pin_(pin),
      readPeriodMs_(cfg.readPeriodMs),
      rawDry_(cfg.rawDry),
      rawWet_(cfg.rawWet),
      thresholdPercent_(cfg.thresholdPercent),
      hysteresisPercent_(cfg.hysteresisPercent),
      friendlyName_(cfg.friendlyName),
      location_(cfg.location)
{}

bool SoilMoistureSensor::begin() {
    pinMode(pin_, INPUT);
    healthy_ = true;
    lastReadMs_ = millis();
    return true;
}

void SoilMoistureSensor::loop() {
    if (!enabled_) return;
    if (!healthy_) return;

    if (millis() - lastReadMs_ < readPeriodMs_)
        return;

    lastReadMs_ = millis();

    // RAW value
    lastRaw_ = analogRead(pin_);

    // Voltage (opcional)
    lastVoltage_ = (lastRaw_ / 4095.0f) * 3.3f;

    // Convertir RAW → porcentaje
    int percent = map(lastRaw_, rawDry_, rawWet_, 0, 100);
    lastPercent_ = constrain(percent, 0, 100);

    // Histeresis para estado seco
    if (!dryState_ && lastPercent_ <= thresholdPercent_) {
        dryState_ = true;
    }
    else if (dryState_ && lastPercent_ >= thresholdPercent_ + hysteresisPercent_) {
        dryState_ = false;
    }

    healthy_ = !isnan(lastPercent_);
}
