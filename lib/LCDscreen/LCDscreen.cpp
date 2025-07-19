#include <LCDscreen.h>

LCDScreen::LCDScreen(uint8_t address, int cols, int rows)
    : lcd(address, cols, rows), cols(cols), rows(rows) {}

void LCDScreen::begin() {
    lcd.init();
    lcd.backlight();
    lcd.clear();
}

void LCDScreen::printLine(int line, const String &message) {
    lcd.setCursor(0, line);
    lcd.print("                "); // Limpia línea
    lcd.setCursor(0, line);
    lcd.print(message);
}

void LCDScreen::startScrolling(int line, const String &message, int delayMs){
    // Comprobamos primero si el mensaje entra en la misma línea sin scroll:
    if(message.length() <= cols){
        printLine(line, message);
        scrollingActive = false;
        return;
    }
    scrollMessage = message + "                ";
    scrollLine = line;
    scrollDelay = delayMs;
    scrollIndex = 0;
    scrollingActive = true;
    lastScrollTime = millis();
}

void LCDScreen::updateScrolling() {
    if (!scrollingActive) return;

    unsigned long now = millis();

    if (now - lastScrollTime >= scrollDelay) {
        // Comprobamos si ya hemos mostrado todo el mensaje + espacio vacío
        if (scrollIndex > scrollMessage.length()) {
            scrollingActive = false;
            lcd.setCursor(0, scrollLine);
            lcd.print(String(" ", cols));  // Limpia la línea tras terminar
            return;
        }

        // Mostrar la subcadena desplazada
        lcd.setCursor(0, scrollLine);
        lcd.print(scrollMessage.substring(scrollIndex, scrollIndex + cols));
        scrollIndex++;
        lastScrollTime = now;
    }
}


void LCDScreen::stopScrolling(){
    scrollingActive = false;
    lcd.setCursor(0, scrollLine);
    lcd.print(String(" ", cols));
}

void LCDScreen::printCentered(int line, const String &message) {
    int padding = (cols - message.length()) / 2;
    lcd.setCursor(padding > 0 ? padding : 0, line);
    lcd.print(message);
}

void LCDScreen::clear() {
    lcd.clear();
}


