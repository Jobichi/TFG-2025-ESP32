#pragma once

#include <IConnection.h>

class ConnectionManager : IConnection{
    protected:
        unsigned long _timeout;
        int _retries;

    public:
        ConnectionManager(unsigned long timeout = 60000, int retries = 3)
            : _timeout(timeout), _retries(retries){}

        virtual ~ConnectionManager() = default;
};