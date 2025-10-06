#pragma once
#include <Arduino.h>
#include "configCredentials.h"

class ActuatorBase{
    public:
        virtual ~ActuatorBase() {}

        // Ciclo de vida.
        virtual bool begin() = 0;
        virtual void loop() = 0;

        // Identificación del Actuador.
        virtual const char* name() const = 0;
        virtual const char* location() const = 0;

        // Control del actuador.
        virtual void on() = 0;
        virtual void off() = 0;

        // Estado del actuador.
        virtual bool isActive() const = 0;  

        // Estado como String:
        virtual String stateString() const = 0;

        // Método para aplicación de comandos recibidos:
        virtual bool applyCommand(const char* command) = 0;
};