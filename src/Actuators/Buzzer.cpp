#include "Actuators/Buzzer.h"
#include "configCredentials.h"

Buzzer::Buzzer(uint8_t pin, const char* friendlyName)
    : pin_(pin), friendlyName_(friendlyName) {} 

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