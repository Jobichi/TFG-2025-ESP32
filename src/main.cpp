#include <Arduino.h>
#include "Screens/OledManager.h"
#include "Sensors/LdrSensor.h"

// Pines
#define LDR_PIN 34
#define LED_PIN 25   // LED blanco (con resistencia y transistor a 5V)

// Crear sensor LDR
LdrSensor ldr(LDR_PIN, "LDR", 300);

// Umbrales con histéresis
const int UMBRAL_ON  = 30;  // % de luz para activar LED
const int UMBRAL_OFF = 40;  // % para apagar LED (evita parpadeos)

void setup() {
    Serial.begin(115200);

    // Inicializar OLED
    setupOLED();
    oledSetTopText("Prueba LDR+LED", 1);
    oledSetBottomText("Iniciando...", 1);
    delay(1500);

    // Inicializar LDR
    ldr.begin();

    // Configuración LED
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
}

void loop() {
    static bool ledOn = false;

    // Actualizar LDR
    ldr.loop();
    int luz = ldr.lastPercent();

    // Lógica con histéresis
    if (!ledOn && luz < UMBRAL_ON) ledOn = true;
    if ( ledOn && luz > UMBRAL_OFF) ledOn = false;

    digitalWrite(LED_PIN, ledOn ? HIGH : LOW);

    // Debug
    Serial.printf("LDR: %d%% | LED: %s\n", luz, ledOn ? "ON" : "OFF");

    // OLED
    String top = "Luz: " + String(luz) + "%";
    String bottom = ledOn ? "Modo Noche → LED ON" : "Dia → LED OFF";
    oledSetTopText(top, 1);
    oledSetBottomText(bottom, 1);
    oledHandle();

    delay(200);
}
