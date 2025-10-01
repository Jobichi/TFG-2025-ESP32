#include "Sensors/MqSensor.h"
#include "configCredentials.h"

MqSensor::MqSensor(
    uint8_t pin,
    unsigned long readPeriodMs,
    float thresholdV,
    const char* friendlyName,
    const char* location
) : pin_(pin),
    readPeriodMs_(readPeriodMs),
    thresholdV_(thresholdV),
    friendlyName_(friendlyName),
    location_(location) {}

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

std::map<String, float> MqSensor::readValues() {
    std::map<String, float> data;
    data["raw"] = lastRaw_;
    data["volt"] = lastVoltage_;
    data["trigger"] = triggered() ? 1.0f : 0.0f;
    return data;
}

bool MqSensor::triggered() const {
    return lastVoltage_ >= thresholdV_;
}