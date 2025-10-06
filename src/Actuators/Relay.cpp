#include "Actuators/Relay.h"

Relay::Relay(
    uint8_t pin, 
    bool activeLow, 
    const char* friendlyName,
    const char* location
) : pin_(pin), 
    activeLow_(activeLow), 
    friendlyName_(friendlyName),
    location_(location) {}

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

bool Relay::applyCommand(const char* command) {
    if (strcmp(command, "ON") == 0) {
        on();
        return true;
    }
    if (strcmp(command, "OFF") == 0) {
        off();
        return true;
    }

    if (strcmp(command, "TOGGLE") == 0) {
        if (active_) off();
        else on();
        return true;
    }

    return false;
}
