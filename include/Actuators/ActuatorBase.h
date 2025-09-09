#pragma once
#include <Arduino.h>

class ActuatorBase{
    public:
        virtual ~ActuatorBase() {}
        virtual bool begin() = 0;
        virtual void loop() = 0;
        virtual const char* name() const = 0;

        // Activación del actuador.
        virtual void on() = 0;

        // Desactivación del actuador.
        virtual void off() = 0;

        // Estado del actuador.
        virtual bool isActive() const = 0;  
};