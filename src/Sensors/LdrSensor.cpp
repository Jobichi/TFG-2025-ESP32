#include "Sensors/LdrSensor.h"
#include "configCredentials.h"  // Para DEBUG

LdrSensor::LdrSensor(uint8_t pin,
                     const char* friendlyName,
                     unsigned long readPeriodMs)
: pin_(pin),
  friendlyName_(friendlyName),
  readPeriodMs_(readPeriodMs) {}

bool LdrSensor::begin() {
    // Configurar ADC
    analogSetPinAttenuation(pin_, ADC_11db);  // rango ~0–3.3V
    healthy_ = true;
    lastReadMs_ = 0;
    return true;
}

void LdrSensor::loop() {
    unsigned long now = millis();
    if (now - lastReadMs_ < readPeriodMs_) return;
    lastReadMs_ = now;

    lastRaw_ = analogRead(pin_);
    lastPercent_ = map(lastRaw_, 0, 4095, 0, 100);

    #if DEBUG
        Serial.printf("[%s] RAW: %d | %d%%\n", friendlyName_, lastRaw_, lastPercent_);
    #endif
}
