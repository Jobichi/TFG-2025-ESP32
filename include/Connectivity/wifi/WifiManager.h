#pragma once

#include <WiFi.h>
#include <Connectivity/Connectivity.h>


class WifiManager : public Connectivity {
    public:
        WifiManager();
        void begin() override;
        void loop() override;
        bool isConnected() override;

    private:
        unsigned long lastReconnectAttempt_ = 0;
        const unsigned long reconnectInterval_ = 5000;
};