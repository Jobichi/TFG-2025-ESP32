#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <string>

#include "interfaces/INetworkManager.hpp"
#include "networks/NetworkConfig.hpp"
#include "core/NetworkState.hpp"

class WiFiManager : public INetworkManager {
public:
    explicit WiFiManager(const NetworkConfig& config);

    // INetworkManager
    bool connect() override;          // inicia intento (no bloquea)
    bool isConnected() const override;
    void disconnect() override;

    // Ciclo de vida
    void begin();   // aplica config (DHCP/IP fija, hostname) y lanza primer intento
    void tick();    // llamar en loop(): gestiona estados / reintentos / mDNS

    // Getters informativos
    String getSSID()    const { return WiFi.SSID(); }
    String getIP()      const { return WiFi.localIP().toString(); }
    String getGateway() const { return WiFi.gatewayIP().toString(); }
    String getMask()    const { return WiFi.subnetMask().toString(); }
    String getDNS1()    const { return WiFi.dnsIP(0).toString(); }
    String getDNS2()    const { return WiFi.dnsIP(1).toString(); }

    // Setters de configuración (para uso antes de begin() o si cambias política)
    void setStaticIP(const IPAddress& localIP, const IPAddress& gatewayIP, const IPAddress& mask);
    bool setDHCP(bool activate = true);
    void setHostname(const String& hn);
    void enableMDNS(bool enabled);

    // Parámetros de reintento / timeouts
    void setConnectTimeoutMs(uint32_t ms) { _connectTimeoutMs = ms; }
    void setBackoffBaseMs(uint32_t ms)    { _backoffBaseMs = ms; }
    void setBackoffMaxMs(uint32_t ms)     { _backoffMaxMs  = ms; }

    NetworkState getState() const { return _state; }

private:
    // Helpers
    void _applyNetConfigOnce();             // aplica DHCP/IP fija + hostname una sola vez
    void _startMDNSIfNeeded();
    void _stopMDNSIfStarted();
    void _scheduleNextAttempt();            // calcula siguiente instante para reintentar
    bool _attemptTimedOut() const;          // timeout del intento actual

    // Util: comparación segura con overflow
    static bool _due(uint32_t t) { return (int32_t)(millis() - t) >= 0; }

private:
    NetworkConfig _config;
    NetworkState  _state = NetworkState::DISCONNECTED;

    bool _netcfgApplied = false;
    bool _mdnsStarted   = false;

    // Reintentos / timeouts
    uint8_t  _tries           = 0;
    uint32_t _nextAttemptMs   = 0;
    uint32_t _attemptDeadline = 0;

    // Defaults razonables para campo
    uint32_t _connectTimeoutMs = 10000; // 10 s para cada intento
    uint32_t _backoffBaseMs    = 1000;  // 1 s
    uint32_t _backoffMaxMs     = 30000; // 30 s
};
