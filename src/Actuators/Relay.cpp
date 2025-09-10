#include "Actuators/Relay.h"
# include "configCredentials.h"

Relay::Relay(
    uint8_t pin, bool activeLow, const char* friendlyName
) : pin_(pin), activeLow_(activeLow), friendlyName_(friendlyName) {}

bool Relay::begin() {
    pinMode(pin_, OUTPUT);
    off();
    return true;
}

void Relay::loop() {}

void Relay::on() {
    digitalWrite(pin_, activeLow_ ? LOW : HIGH);
    active_ = true;

    #if DEBUG
        Serial.printf("[%s] ON\n", friendlyName_);
    #endif
}

void Relay::off() {
    digitalWrite(pin_, activeLow_ ? HIGH : LOW);
    active_ = false;

    #if DEBUG
        Serial.printf("[%s] OFF\n", friendlyName_);
    #endif
}

