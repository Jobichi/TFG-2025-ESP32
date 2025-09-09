#include "Sensors/SoilMoistureSensor.h"
#include "configCredentials.h"

SoilMoistureSensor::SoilMoistureSensor(
    uint8_t pin,
    unsigned long readPeriodMs,
    int rawDry,
    int rawWet,
    int thresholdPercent,
    int hysteresisPercent,
    const char* friendlyName
) : pin_(pin),
    readPeriodMs_(readPeriodMs),
    rawDry_(rawDry),
    rawWet_(rawWet),
    thresholdPercent_(thresholdPercent),
    hysteresisPercent_(hysteresisPercent),
    friendlyName_(friendlyName) {}

bool SoilMoistureSensor::begin(){
    pinMode(pin_, INPUT);
    healthy_ = true;
    lastReadMs_ = 0;
    return true;
}

void SoilMoistureSensor::loop(){
    const unsigned long now = millis();
    if(now - lastReadMs_ < readPeriodMs_) return;
    lastReadMs_ = now;

    lastRaw_ = analogRead(pin_);
    lastVoltage_ = (lastRaw_ / 4095.0) * 3.3;
    lastPercent_ = map(lastRaw_, rawDry_, rawWet_, 0, 100);
    lastPercent_ = constrain(lastPercent_, 0, 100);

    // Histeresis: control de estado entre seco y humedo.
    if(!dryState_ && lastPercent_ <= (thresholdPercent_ - hysteresisPercent_)){
        dryState_ = true;   // Suelo Seco
    } else if (dryState_ && lastPercent_ >= (thresholdPercent_ - hysteresisPercent_)){
        dryState_ = false; // Suelo Humedo
    }

    healthy_ = true;

    #if DEBUG
        Serial.printf("[%s] Raw=%d, Volt=%.2f V, Hum=%d%%, Estado=%s\n",
                      friendlyName_, lastRaw_, lastVoltage_, lastPercent_,
                      dryState_ ? "SECO" : "HUMEDO");
    #endif
}