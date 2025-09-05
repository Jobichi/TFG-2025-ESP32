#include "Screens/LcdManager.h"

// --- Modificar para otros modelos de LCD ---
#define COLS 16
#define LINES 2

LiquidCrystal_I2C lcd(0x27, COLS, LINES);

void setupLCD() {
    lcd.init();
    lcd.backlight();
    lcd.clear();
}

// --- Helper interno ---
static void scrollText(int row, const String& text, int delayMs = 300) {
    int len = text.length();
    if (len <= COLS) {  
        lcd.setCursor(0, row);
        lcd.print(text);
        return;
    }

    // Ventana deslizante
    for (int i = 0; i <= len - COLS; i++) {
        lcd.setCursor(0, row);
        lcd.print(text.substring(i, i + COLS));
        delay(delayMs);
    }
}

void lcdPrint(const String& line1, const String& line2) {
    lcd.clear();
    scrollText(0, line1);
    if (line2.length() > 0) {
        scrollText(1, line2);
    }
}

void lcdClear() {
    lcd.clear();
}