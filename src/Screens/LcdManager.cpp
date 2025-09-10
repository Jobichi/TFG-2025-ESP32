#include "Screens/LcdManager.h"

#define COLS 16
#define LINES 2

LiquidCrystal_I2C lcd(0x27, COLS, LINES);

static String currentLine1 = "";
static String currentLine2 = "";
static bool scrolling = false;
static unsigned long lastScroll = 0;
static const unsigned long scrollInterval = 400; // velocidad del scroll

void setupLCD() {
    lcd.init();
    lcd.backlight();
    lcd.clear();
}

void lcdPrint(const String& line1, const String& line2) {
    lcd.clear();
    currentLine1 = line1;
    currentLine2 = line2;

    // Si las dos líneas caben en 16 chars → se escriben directamente
    if (line1.length() <= COLS && line2.length() <= COLS) {
        lcd.setCursor(0, 0);
        lcd.print(line1);
        lcd.setCursor(0, 1);
        lcd.print(line2);
        scrolling = false;
    } else {
        // Escribimos las dos líneas completas
        lcd.setCursor(0, 0);
        lcd.print(line1);
        lcd.setCursor(0, 1);
        lcd.print(line2);

        // Activamos el scroll hardware
        scrolling = true;
        lcd.home(); // aseguramos cursor al inicio
    }
}

void lcdClear() {
    lcd.clear();
    scrolling = false;
    currentLine1 = "";
    currentLine2 = "";
}

// Debe llamarse en cada loop()
void lcdHandle() {
    if (scrolling) {
        unsigned long now = millis();
        if (now - lastScroll >= scrollInterval) {
            lastScroll = now;
            lcd.scrollDisplayLeft(); // mueve todo el contenido
        }
    }
}
