#include <Arduino.h>
#include "configCredentials.h"

#include "Connectivity/WifiManager.h"
#include "Screens/OledManager.h"

// === MQ-2 Sensor ===
#include "Sensors/MqSensor.h"

// Pin 34, muestreo cada 1s, umbral = 1.0 V
MqSensor mq(34, 1000, 1.0, "MQ-2");

void setup() {
    Serial.begin(115200);

    // WiFi y OLED
    setupWifi();
    setupOLED();
    oledSetTopText("ESP32 + MQ-2", 1);

    // Inicializar sensor
    mq.begin();
}

void loop() {
    handleWifi();
    oledHandle();

    // Actualizar MQ-2
    mq.loop();

    // Mostrar cada 1s
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 1000) {
        lastPrint = millis();

        float volt = mq.lastVoltage();
        int raw = mq.lastRaw();

        // Serial
        Serial.printf("[%s] Raw=%d, Volt=%.2f V",
                      mq.name(), raw, volt);
        if (mq.triggered()) {
            Serial.print(" [TRIGGERED]");
        }
        Serial.println();

        // OLED
        String topLine = "V: " + String(volt, 2) + " V";
        oledSetTopText(topLine, 2);

        if (mq.triggered()) {
            oledSetBottomText("GAS DETECTADO", 1);
        } else {
            String bottomLine = "ADC: " + String(raw);
            oledSetBottomText(bottomLine, 1);
        }
    }
}
