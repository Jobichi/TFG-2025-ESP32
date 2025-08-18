// include/interfaces/INetworkManager.hpp

#pragma once

class INetworkManager {
public:
    virtual ~INetworkManager() = default;
    virtual bool connect() = 0;           // no bloqueante
    virtual bool isConnected() const = 0;
    virtual void disconnect() = 0;
};
