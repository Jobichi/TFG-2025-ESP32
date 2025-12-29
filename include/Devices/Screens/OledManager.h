#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Inicializa la pantalla OLED
void setupOLED();

// Limpia toda la pantalla
void oledClear();

// Define el texto de la parte superior (scroll infinito si es largo)
// - text: cadena a mostrar
// - textSize: tamaño de fuente (1 = normal, 2 = doble, etc.)
void oledSetTopText(const String& text, int textSize = 1);

// Define el texto de la parte inferior (fijo, independiente)
// - text: cadena a mostrar
// - textSize: tamaño de fuente
void oledSetBottomText(const String& text, int textSize = 1);

// Refresca la pantalla y avanza el scroll si es necesario
// 👉 Esta función debe llamarse en cada loop()
void oledHandle();