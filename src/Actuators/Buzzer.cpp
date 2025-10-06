#include "Actuators/Buzzer.h"

Buzzer::Buzzer(uint8_t pin, const char* friendlyName, const char* location)
    : pin_(pin), friendlyName_(friendlyName), location_(location) {} 

bool Buzzer::begin(){
    pinMode(pin_, OUTPUT);
    off();
    return true;
}

void Buzzer::loop() {}

void Buzzer::on() {
    digitalWrite(pin_, HIGH);
    active_ = true;
    #if DEBUG
        Serial.printf("[%s] ON\n", friendlyName_);
    #endif
}

void Buzzer::off() {
    digitalWrite(pin_, LOW);
    active_ = false;
    #if DEBUG
        Serial.printf("[%s] OFF\n", friendlyName_);
    #endif
}

bool Buzzer::applyCommand(const char* command) {
    if (strcmp(command, "ON") == 0) {
        on();
        return true;
    }
    if (strcmp(command, "OFF") == 0) {
        off();
        return true;
    }
    return false; 
}