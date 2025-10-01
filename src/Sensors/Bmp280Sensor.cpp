#include "Sensors/Bmp280Sensor.h"
#include "configCredentials.h"

Bmp280Sensor::Bmp280Sensor(
    uint8_t i2cAddress,
    unsigned long readPeriodMs,
    const char* friendlyName,
    const char* location)
: i2cAddress_(i2cAddress),
  readPeriodMs_(readPeriodMs),
  friendlyName_(friendlyName),
  location_(location) {}

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

std::map<String, float> Bmp280Sensor::readValues() {
    std::map<String, float> data;
    data["temp"] = lastT_;
    data["pres"] = lastP_;
    return data;
}

String Bmp280Sensor::stateString() {
    if(!isHealthy()) return "ERROR";
    return String(lastT_, 1) + "ºC " + String(lastP_, 1) + "hPa";
}
