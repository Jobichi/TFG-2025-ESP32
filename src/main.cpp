#include <Arduino.h>
#include "Screens/LcdManager.h"

unsigned long lastChange = 0;
const unsigned long changeInterval = 16000; // cambiar mensaje cada 8s
int messageIndex = 0;

void setup() {
    Serial.begin(115200);
    setupLCD();
    lcdPrint("Inicializando...", "LCD Manager");
}

void loop() {
    lcdHandle(); // 👉 refresca el scroll hardware si está activo

    unsigned long now = millis();
    if (now - lastChange >= changeInterval) {
        lastChange = now;

        switch (messageIndex) {
            case 0:
                lcdPrint("Hola mundo", "ESP32 con LCD");
                break;
            case 1:
                lcdPrint("Este es un mensaje mucho mas largo que 16 caracteres", 
                         "Scroll hardware activo");
                break;
            case 2:
                lcdPrint("TFG Integracion IoT con ESP32", 
                         "Sensores y Actuadores");
                break;
            case 3:
                lcdPrint("Mensaje corto", "Tambien corto");
                break;
        }

        messageIndex = (messageIndex + 1) % 4;
    }
}
