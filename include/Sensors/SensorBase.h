#pragma once
#include <Arduino.h>
#include <map>
class SensorBase{
    public:
        virtual ~SensorBase(){}
        virtual bool begin() = 0;
        virtual void loop() = 0;
        virtual bool isHealthy() = 0;
        virtual const char* name() const = 0;

        // Mapa clave-valor para lecturas:
        virtual std::map<String, float> readValues() = 0;
};