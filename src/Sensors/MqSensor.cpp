#include "Sensors/MqSensor.h"
#include "configCredentials.h"

MqSensor::MqSensor(
    uint8_t pin,
    unsigned long readPeriodMs,
    float thresholdV,
    const char* friendlyName
) : pin_(pin),
    readPeriodMs_(readPeriodMs),
    thresholdV_(thresholdV),
    friendlyName_(friendlyName) {}

bool MqSensor::begin(){
    pinMode(pin_, INPUT);
    healthy_ = true;
    lastReadMs_ = 0;
    return true;
}

void MqSensor::loop(){
    const unsigned long now = millis();
    if(now - lastReadMs_ < readPeriodMs_) return;
    lastReadMs_ = now;

    lastRaw_ = analogRead(pin_);
    lastVoltage_ = (lastRaw_ / 4095.0) * 3.3;

    healthy_ = true;

    #if DEBUG
        Serial.printf("[%s] Raw: %d, Volt: %.2f V",
                      friendlyName_, lastRaw_, lastVoltage_);
        if (triggered()) {
            Serial.print(" [TRIGGERED]");
        }
        Serial.println();
    #endif
}