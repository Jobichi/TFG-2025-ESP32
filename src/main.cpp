#include <Arduino.h>
#include "configCredentials.h"

#include "Connectivity/WifiManager.h"
#include "Screens/OledManager.h"

// === Pin del relé ===
const int RELAY_PIN = 5;   // usa un pin digital libre (ej. GPIO4)

// ⚠️ Nota: algunos módulos de relé se activan con LOW en lugar de HIGH
// Ajusta ACTIVE_STATE a HIGH o LOW según tu módulo
const int ACTIVE_STATE = HIGH;

void setup() {
    Serial.begin(115200);

    setupWifi();
    setupOLED();
    oledSetTopText("ESP32 + Relay", 1);

    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, !ACTIVE_STATE); // empezar apagado
}

void loop() {
    handleWifi();
    oledHandle();

    static unsigned long lastToggle = 0;
    static bool state = false;

    if (millis() - lastToggle > 2000) {  // cambiar cada 2s
        lastToggle = millis();
        state = !state;

        if (state) {
            digitalWrite(RELAY_PIN, ACTIVE_STATE);
            oledSetBottomText("Relay ON", 1);
        } else {
            digitalWrite(RELAY_PIN, !ACTIVE_STATE);
            oledSetBottomText("Relay OFF", 1);
        }
    }
}
