#include <Arduino.h>
#include "configCredentials.h"

#include "Connectivity/WifiManager.h"
#include "Screens/OledManager.h"

// === Sensor BMP280 ===
#include "Sensors/Bmp280Sensor.h"

// Instancia del BMP280 (dirección I2C, periodo de lectura, nombre)
Bmp280Sensor bmp(0x76, 2000, "BMP280");

void setup() {
    Serial.begin(115200);

    // WiFi y OLED
    setupWifi();
    setupOLED();
    oledSetTopText("ESP32 listo", 1);

    // Inicializar sensor
    bmp.begin();
}

void loop() {
    handleWifi();
    oledHandle();

    // Actualizar lecturas del sensor
    bmp.loop();

    // Mostrar estado cada 2s
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 2000) {
        lastPrint = millis();

        if (!bmp.isHealthy()) {
            oledSetBottomText("Lectura fallida", 1);
            Serial.println("[BMP280] Error de lectura");
        } else {
            String topLine = "T: " + String(bmp.lastTemperatureC(), 1) + " C";
            String bottomLine = "P:" + String(bmp.lastPressure(), 0) + " hPa";

            oledSetTopText(topLine, 2);
            oledSetBottomText(bottomLine, 1);

            Serial.printf("[%s] T=%.2f C, P=%.2f hPa\n",
                          bmp.name(),
                          bmp.lastTemperatureC(),
                          bmp.lastPressure());
        }
    }
}
