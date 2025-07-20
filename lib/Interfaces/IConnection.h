#pragma once

/*
    Esta es una interfaz que define los atributos que se deben
    de configurar a la hora de llevar a cabo la programación de
    los diferentes métodos de conexión existentes.
*/

class IConnection{
    public:
        virtual void connect() = 0;
        virtual void checkConnection() = 0;
        virtual bool isConnected() = 0;
        virtual ~IConnection() = default;
};