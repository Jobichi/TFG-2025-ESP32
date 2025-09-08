#include "Sensors/Bmp280Sensor.h"
#include "configCredentials.h"

Bmp280Sensor::Bmp280Sensor(uint8_t i2cAddress,
                           unsigned long readPeriodMs,
                           const char* friendlyName)
: i2cAddress_(i2cAddress),
  readPeriodMs_(readPeriodMs),
  friendlyName_(friendlyName) {}

bool Bmp280Sensor::begin() {
    if (!bmp_.begin(i2cAddress_)) {
        #if DEBUG
            Serial.printf("[%s] No se pudo encontrar el BMP280 en 0x%X\n", friendlyName_, i2cAddress_);
        #endif
        healthy_ = false;
        return false;
    }
    healthy_ = true;
    lastReadMs_ = 0;
    return true;
}

void Bmp280Sensor::loop() {
    const unsigned long now = millis();
    if (now - lastReadMs_ < readPeriodMs_) return;
    lastReadMs_ = now;

    float t = bmp_.readTemperature();
    float p = bmp_.readPressure() / 100.0F; // Conversión a hPa

    if (isnan(t) || isnan(p)) {
        healthy_ = false;
        #if DEBUG
            Serial.printf("[%s] Error de lectura.\n", friendlyName_);
        #endif
        return;
    }

    healthy_ = true;
    lastT_ = t;
    lastP_ = p;

    #if DEBUG
        Serial.printf("[%s] T: %.2f °C, P: %.2f hPa\n",
                      friendlyName_, lastT_, lastP_);
    #endif
}
