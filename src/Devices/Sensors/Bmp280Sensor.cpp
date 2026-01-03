#include <Devices/Sensors/Bmp280Sensor.h>

Bmp280Sensor::Bmp280Sensor(
    const Bmp280Config& cfg
)
    : i2cAddress_(cfg.i2cAddress),
      readPeriodMs_(cfg.readPeriodMs),
      friendlyName_(cfg.friendlyName),
      location_(cfg.location)
{}

bool Bmp280Sensor::begin() {

    if (!bmp_.begin(i2cAddress_)) {
        Serial.println("[BMP280] Error: no se encontró el sensor BMP280.");
        healthy_ = false;
        return false;
    }

    bmp_.setSampling(
        Adafruit_BMP280::MODE_NORMAL,
        Adafruit_BMP280::SAMPLING_X2,   // temperatura (interna)
        Adafruit_BMP280::SAMPLING_X16,  // presión
        Adafruit_BMP280::FILTER_X16,
        Adafruit_BMP280::STANDBY_MS_500
    );

    healthy_ = true;
    lastReadMs_ = millis();
    return true;
}

void Bmp280Sensor::loop() {
    if (!enabled_) return;     // Si no esta enabled -> Salida
    if (!healthy_) return;      // Si no está listo o contiene errores -> Salida

    // Si no ha cumplido el tiempo para nueva lectura -> Salida
    if (millis() - lastReadMs_ < readPeriodMs_)
        return;

    lastReadMs_ = millis();

    // Lecturas directas del BMP280
    lastT_ = bmp_.readTemperature();   // no se usa como value, solo info
    lastP_ = bmp_.readPressure() / 100.0f;  // Conversión a hPa

    healthy_ = !isnan(lastP_);
}
