#include <Devices/Sensors/PirSensor.h>

PirSensor::PirSensor(
    uint8_t pin,
    const PirConfig &cfg
)
    : pin_(pin),
      activeHigh_(cfg.activeHigh),
      usePulldown_(cfg.usePulldown),
      warmupMs_(cfg.warmupMs),
      stableMs_(cfg.stableMs),
      friendlyName_(cfg.friendlyName),
      location_(cfg.location)
{}

bool PirSensor::begin() {
    pinMode(pin_, usePulldown_ ? INPUT_PULLDOWN : INPUT);
    bootMs_ = millis();
    healthy_ = true;
    lastStable_ = false;
    return true;
}

void PirSensor::loop() {
    if (!enabled_) return;
    if (!healthy_) return;

    // Warm-up
    if (millis() - bootMs_ < warmupMs_) return;

    bool raw = digitalRead(pin_);
    bool active = activeHigh_ ? raw : !raw;

    // Debounce / Estabilidad
    if (active != lastStable_) {
        if (!candidate_) {
            candidate_ = true;
            candidateMs_ = millis();
        }

        if (millis() - candidateMs_ >= stableMs_) {
            lastStable_ = active;
            candidate_ = false;

            motion_ = active;
            lastChangeMs_ = millis();
        }
    } else {
        candidate_ = false;
    }
}
