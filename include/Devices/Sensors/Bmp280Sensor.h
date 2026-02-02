#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Devices/Sensors/Sensor.h>
#include <Adafruit_BMP280.h>

/*
    Clase BMP280SENSOR:
    1. Se encarga de la configuración completa para el sensor de presión.
    2. Dependiendo de su estado, realizará unas acciones u otras.
    3. Usa la dirección I2C por defecto, pero se puede modificar.
*/

struct Bmp280Config {
    uint8_t i2cAddress = 0x76;
    unsigned long readPeriodMs = 2000;
    const char* friendlyName = "BMP280 Pressure";
    const char* location = "room-name";
};

class Bmp280Sensor : public Sensor {
public:
    // Constructor
    explicit Bmp280Sensor(const Bmp280Config &cfg = {});

    // Configuración inicial del sensor
    bool begin() override;
    // Funcionamiento del sensor en el loop
    void loop() override;

    // Comprobar estado del sensor
    bool isHealthy() const override { return healthy_; }

    // Métodos getter
    const char* name() const override { return friendlyName_.c_str(); }
    const char* location() const override { return location_.c_str(); }
    bool isEnabled() const override { return enabled_; }
    
    // Activación para SET
    void setEnabled(bool enable) override { enabled_ = enable; }

    // Valor principal → PRESIÓN hPa
    float getValue() const override { return lastP_; }
    const char* getUnits() const override { return "hPa"; }

    // Extras de depuración
    float lastPressure() const { return lastP_; }
    float lastTemperatureC() const { return lastT_; }

private:
    uint8_t i2cAddress_;
    unsigned long readPeriodMs_;
    String friendlyName_;
    String location_;

    Adafruit_BMP280 bmp_;

    bool enabled_{true}; // Activado inicialmente
    bool healthy_{false}; 

    // Sin valores de presión inicialmente
    float lastT_{NAN}; 
    float lastP_{NAN};
    unsigned long lastReadMs_{0};   // Tiempo desde la última lectura
};
