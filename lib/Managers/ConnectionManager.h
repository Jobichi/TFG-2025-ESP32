#pragma once

/*
    Esta clase permite unificar la estructura minima en la que las diferentes
    conexiones (WiFi, MQTT, BlueTooth...) se deberían de definir para
    un buen control de fallos y reintentos.
*/

class ConnectionManager{
    protected:
        // Parametros para control de reintentos y no bloqueo:
        unsigned long time;
        int maxRetries;

    public:
        // Constructor de clase.
        ConnectionManager(unsigned long time = 150000, int maxRetries = 5)
            : time(time), maxRetries(maxRetries){}
        // Parametros a definir en las clases hijas:
        virtual void connect() = 0;
        virtual void checkConnection() = 0;
        virtual void isConnected() = 0;
        virtual ~ConnectionManager(){}
};