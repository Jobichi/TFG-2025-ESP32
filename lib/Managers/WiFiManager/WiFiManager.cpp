#include <Arduino.h>
#include <WiFiManager/WiFiManager.h>

WifiManager::WifiManager(const char *ssid, const char *password, int ledPin,
                         unsigned long time, int retries, LCDScreen* screen)
    : ConnectionManager(time, retries), _ssid(ssid), _password(password),
      _ledPin(ledPin), _screen(screen) {
    pinMode(_ledPin, OUTPUT);
    digitalWrite(_ledPin, LOW);
}

// Mostrar mensajes en pantalla si existe
void WifiManager::showMessage(const String& line1, const String& line2, bool scroll) {
    if (!_screen) return;
    _screen->clear();
    if (scroll && line1.length() > 16) {
        _screen->startScrolling(0, line1);
    } else {
        _screen->printCentered(0, line1);
    }
    if (line2 != "") {
        _screen->printCentered(1, line2);
    }
}

// Conexión a la WiFi:
void WifiManager::connect() {
    Serial.println("Iniciando la conexión WiFi...");
    showMessage(String("Conectando: ") + _ssid, "Espere...", true);

    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid, _password);

    unsigned long startAttempTime = millis();
    int attempts = 0;
    
    while (!isConnected() && attempts < _retries) {
        // Parpadeo del LED mientras conecta
        digitalWrite(_ledPin, HIGH);
        delay(250);
        digitalWrite(_ledPin, LOW);
        delay(250);

        if (millis() - startAttempTime > _timeout) {
            attempts++;
            startAttempTime = millis();
            Serial.printf("Reintentando conexión (%d/%d)\n", attempts, _retries);
            WiFi.disconnect();
            WiFi.begin(_ssid, _password);
        }
    }

    if (isConnected()) {
        Serial.println("Conectado al WiFi!");
        Serial.print("IP asignada: ");
        Serial.println(WiFi.localIP());
        digitalWrite(_ledPin, HIGH);
        showMessage("Conectado!", WiFi.localIP().toString());
    } else {
        Serial.println("No se pudo conectar al WiFi.");
        digitalWrite(_ledPin, LOW);
        showMessage("Error WiFi");
    }
}

bool WifiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void WifiManager::checkConnection() {
    if (!isConnected()) {
        Serial.println("WiFi desconectado. Reintentando conexión...");
        showMessage("Reconectando...", _ssid, true);
        connect();
    }
}
