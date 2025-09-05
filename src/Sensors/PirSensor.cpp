#include "Sensors/PirSensor.h"
#include "configCredentials.h"

PirSensor::PirSensor(uint8_t pin,
                     bool activeHigh,
                     bool usePulldown,
                     unsigned long warmupMs,
                     unsigned long stableMs,
                     const char* friendlyName)
: pin_(pin),
  activeHigh_(activeHigh),
  usePulldown_(usePulldown),
  warmupMs_(warmupMs),
  stableMs_(stableMs),
  friendlyName_(friendlyName) {}

bool PirSensor::begin() {
    pinMode(pin_, usePulldown_ ? INPUT_PULLDOWN : INPUT);
    bootMs_ = millis();
    return true;
}

void PirSensor::loop() {
    const unsigned long now = millis();
    if (now - bootMs_ < warmupMs_) {
        healthy_ = false;
        #if DEBUG
            Serial.printf("[%s] Calibrando...\n", friendlyName_);
        #endif
        return;
    }
    healthy_ = true;

    const int raw = digitalRead(pin_);
    const bool logical = activeHigh_ ? (raw == HIGH) : (raw == LOW);

    if (logical != candidate_) {
        candidate_ = logical;
        candidateMs_ = now;
    }
    if ((now - candidateMs_) >= stableMs_) {
        if (candidate_ != lastStable_) {
            lastStable_  = candidate_;
            motion_      = candidate_;
            lastChangeMs_ = now;

            #if DEBUG
                Serial.printf("[%s] Cambio: %s\n",
                              friendlyName_,
                              motion_ ? "MOVIMIENTO" : "Libre");
            #endif
        }
    }
}
