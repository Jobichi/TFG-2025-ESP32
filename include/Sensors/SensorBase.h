#pragma once
#include <Arduino.h>
#include <map>
class SensorBase{
    public:
        virtual ~SensorBase(){}

        // Inicialización / Ciclo de vida
        virtual bool begin() = 0;
        virtual void loop() = 0;
        virtual bool isHealthy() = 0;

        // Identificación Sensores:
        // virtual int id() const = 0; -> Implementado por arrays
        virtual const char* name() const = 0;
        virtual const char* location() const = 0;

        // Mapa clave-valor para lecturas:
        virtual std::map<String, float> readValues() = 0;

        // Estados resumidos para anuncios:
        virtual String stateString() = 0;
};