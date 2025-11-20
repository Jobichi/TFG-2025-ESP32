#pragma once
#include <Constants.h>

class Connectivity{
    public:
        // Inicialización del modulo de conectividad.
        virtual void begin() = 0;

        // Bucle de mantenimiento (reconexiones, eventos, etc)
        virtual void loop() = 0;

        // Estado actual de la conectividad.
        virtual bool isConnected() = 0;

        // Destructor virtual.
        virtual ~Connectivity() {}
};