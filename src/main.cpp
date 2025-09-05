#include <Arduino.h>
#include "configCredentials.h"

#include "Connectivity/WifiManager.h"
#include "Screens/OledManager.h"

// === Sensor PIR ===
#include "Sensors/PirSensor.h"

// Instancia del PIR (pin, activeHigh, usePulldown, warmupMs, stableMs, nombre)
PirSensor pir(23, true, false, 60000, 80, "PIR");

void setup() {
    Serial.begin(115200);

    // WiFi y OLED
    setupWifi();
    setupOLED();
    oledSetTopText("ESP32 listo", 1);

    // Inicializar sensor
    pir.begin();
}

void loop() {
    handleWifi();
    oledHandle();

    pir.loop();

    // Mostrar estado en OLED y Serial cada 2s
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 2000) {
        lastPrint = millis();

        if (!pir.isHealthy()) {
            oledSetBottomText("Calibrando...", 1);
        } else {
            if (pir.motion()) {
                oledSetBottomText("MOVIMIENTO", 2);
            } else {
                oledSetBottomText("Libre", 2);
            }
        }
    }
}
