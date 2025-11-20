#include <Devices/Sensors/SoilMoistureSensor.h>

SoilMoistureSensor::SoilMoistureSensor(
    uint8_t pin,
    unsigned long readPeriodMs,
    int rawDry,
    int rawWet,
    int thresholdPercent,
    int hysteresisPercent,
    const char* friendlyName,
    const char* location
)
    : pin_(pin),
      readPeriodMs_(readPeriodMs),
      rawDry_(rawDry),
      rawWet_(rawWet),
      thresholdPercent_(thresholdPercent),
      hysteresisPercent_(hysteresisPercent),
      friendlyName_(friendlyName),
      location_(location)
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
