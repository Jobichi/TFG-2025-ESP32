#include "Sensors/LdrSensor.h"
#include "configCredentials.h"  // Para DEBUG

LdrSensor::LdrSensor(
    uint8_t pin,
    unsigned long readPeriodMs,
    const char* friendlyName,
    const char* location)
: pin_(pin),
  friendlyName_(friendlyName),
  readPeriodMs_(readPeriodMs),
  location_(location) {}

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

std::map<String,float> LdrSensor::readValues() {
    std::map<String,float> data;
    data["raw"] = lastRaw_;
    data["percent"] = lastPercent_;
    return data;
}

String LdrSensor::stateString() {
    if (!healthy_) return "ERROR";
    return String(lastRaw_) + " (" + String(lastPercent_) + "%)";
}
