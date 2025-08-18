#include "networks/WiFiManager.hpp"

WiFiManager::WiFiManager(const NetworkConfig& cfg)
: _config(cfg) {}

void WiFiManager::setStaticIP(const IPAddress& localIP, const IPAddress& gatewayIP, const IPAddress& mask) {
    _config.staticIP = localIP;
    _config.gateway  = gatewayIP;
    _config.subnet   = mask;
    _config.isDHCP   = false;
    _netcfgApplied   = false; // para re-aplicar en begin()
}

bool WiFiManager::setDHCP(bool activate) {
    _config.isDHCP = activate;
    _netcfgApplied = false;
    return true;
}

void WiFiManager::setHostname(const String& hn) {
    _config.hostname = hn;
    _netcfgApplied   = false; // se aplica antes de begin()
}

void WiFiManager::enableMDNS(bool enabled) {
    _config.mdnsEnabled = enabled;
}

void WiFiManager::_applyNetConfigOnce() {
    if (_netcfgApplied) return;

    WiFi.mode(WIFI_STA);
    // Evita mezclar autoreconect del core con tu FSM
    WiFi.setAutoReconnect(false);
    WiFi.persistent(false); // no escribas NVS/flash cada vez

    // Hostname (antes de begin)
    if (_config.hostname.length() > 0) {
        WiFi.setHostname(_config.hostname.c_str());
    }

    if (_config.isDHCP) {
        // Volver a DHCP (ESP32): pasar INADDR_NONE
        WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    } else {
        WiFi.config(_config.staticIP, _config.gateway, _config.subnet,
                    _config.dns1, _config.dns2);
    }

    _netcfgApplied = true;
}

void WiFiManager::_startMDNSIfNeeded() {
    if (_mdnsStarted) return;
    if (!_config.mdnsEnabled) return;
    if (_config.hostname.isEmpty()) return;
    // Solo tras tener IP
    if (WiFi.status() == WL_CONNECTED) {
        if (MDNS.begin(_config.hostname.c_str())) {
            _mdnsStarted = true;
        }
    }
}

void WiFiManager::_stopMDNSIfStarted() {
    if (_mdnsStarted) {
        MDNS.end();
        _mdnsStarted = false;
    }
}

void WiFiManager::_scheduleNextAttempt() {
    uint32_t delayMs = _backoffBaseMs;
    if (_tries > 0) {
        // backoff exponencial con tope
        delayMs = _backoffBaseMs << (_tries - 1);
        if (delayMs > _backoffMaxMs) delayMs = _backoffMaxMs;
    }
    _nextAttemptMs = millis() + delayMs;
    if (_tries < 12) _tries++; // evita overflow del shift
}

bool WiFiManager::_attemptTimedOut() const {
    return _due(_attemptDeadline) && (WiFi.status() != WL_CONNECTED);
}

void WiFiManager::begin() {
    _applyNetConfigOnce();
    connect();               // primer intento no bloqueante
}

bool WiFiManager::connect() {
    if (_config.ssid.isEmpty()) {
        _state = NetworkState::FAILED;
        return false;
    }

    // Si ya está conectada, actualiza estado y listo
    if (WiFi.status() == WL_CONNECTED) {
        _state = NetworkState::CONNECTED;
        _tries = 0;
        _startMDNSIfNeeded();
        return true;
    }

    _applyNetConfigOnce();

    // Evita relanzar begin si ya estamos en CONNECTING con deadline vigente
    if (_state == NetworkState::CONNECTING && !_attemptTimedOut()) {
        return true;
    }

    // Inicia intento (no bloquea)
    _state = NetworkState::CONNECTING;
    WiFi.begin(_config.ssid.c_str(),
               _config.password.length() ? _config.password.c_str() : nullptr);

    _attemptDeadline = millis() + _connectTimeoutMs;
    if (_tries == 0) _tries = 1;   // cuenta primer intento
    return true;
}

bool WiFiManager::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

void WiFiManager::disconnect() {
    WiFi.disconnect(false);     // no apagar radio
    _stopMDNSIfStarted();
    _state = NetworkState::DISCONNECTED;
    _tries = 0;
    // planifica reintento si alguien llama tick() después
    _scheduleNextAttempt();
}

void WiFiManager::tick() {
    const wl_status_t s = WiFi.status();

    switch (_state) {
    case NetworkState::CONNECTED:
        if (s != WL_CONNECTED) {
            // caída de enlace
            _stopMDNSIfStarted();
            _state = NetworkState::DISCONNECTED;
            _scheduleNextAttempt(); // reintento cuando toque
        }
        break;

    case NetworkState::CONNECTING:
        if (s == WL_CONNECTED) {
            _state = NetworkState::CONNECTED;
            _tries = 0;
            _startMDNSIfNeeded();
        } else if (_attemptTimedOut()) {
            // cancelar intento y programar reintento con backoff
            WiFi.disconnect(false);
            _state = NetworkState::DISCONNECTED;
            _scheduleNextAttempt();
        }
        break;

    case NetworkState::DISCONNECTED:
    case NetworkState::FAILED:
    case NetworkState::FATAL_ERROR:
    default:
        // ¿toca reintentar?
        if (_due(_nextAttemptMs)) {
            connect(); // relanza intento; CONNECTING con nuevo deadline
        }
        break;
    }
}
