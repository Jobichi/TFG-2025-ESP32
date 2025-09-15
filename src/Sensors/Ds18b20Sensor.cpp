#include "Sensors/Ds18b20Sensor.h"
#include "configCredentials.h"

Ds18b20Sensor::Ds18b20Sensor(uint8_t pin,
                             unsigned long readPeriodMs,
                             const char* friendlyName)
: pin_(pin),
  readPeriodMs_(readPeriodMs),
  friendlyName_(friendlyName),
  oneWire_(pin),
  sensors_(&oneWire_) {}

bool Ds18b20Sensor::begin() {
    sensors_.begin();
    healthy_ = true;
    lastReadMs_ = 0;
    return true;
}

void Ds18b20Sensor::loop() {
    const unsigned long now = millis();
    if (now - lastReadMs_ < readPeriodMs_) return;
    lastReadMs_ = now;

    sensors_.requestTemperatures();
    const float t = sensors_.getTempCByIndex(0);

    if (t == DEVICE_DISCONNECTED_C || isnan(t)) {
        healthy_ = false;
        #if DEBUG
            Serial.printf("[%s] Error de lectura DS18B20\n", friendlyName_);
        #endif
        return;
    }

    healthy_ = true;
    lastT_ = t;
}
