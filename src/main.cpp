#include <Arduino.h>
#include <LCDscreen.h>

LCDScreen screen;

void setup() {
    screen.begin();
    screen.printCentered(0, "Test de LCD");
    
    delay(2000);  // Esperamos 2 segundos

    screen.startScrolling(1, "Este mensaje es demasiado largo para una sola línea", 300);
}

void loop() {
    screen.updateScrolling();

    // Detenemos el scroll después de 10 segundos
    static unsigned long startTime = millis();
    if (millis() - startTime > 10000 && screen.isScrolling()) {
        screen.stopScrolling();
        screen.printLine(1, "Scroll detenido");
    }
}