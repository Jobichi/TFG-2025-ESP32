#include <Arduino.h>
#include <WifiScanner.h>
#include <LCDscreen.h>

LCDScreen lcd;

void setup() {
    Serial.begin(115200);
    lcd.begin();
    lcd.printCentered(0, "Buscando redes");
    delay(1500);

    auto redes = WifiScanner::scanNetworks();

    if (redes.empty()) {
        lcd.printCentered(0, "Sin redes");
        return;
    }

    for (const auto& net : redes) {
        lcd.clear();

        String ssidLine = net.ssid;
        if (ssidLine.length() > 16) {
            lcd.startScrolling(0, ssidLine);
        } else {
            lcd.printLine(0, ssidLine);
        }

        String info = String("RSSI: ") + net.rssi + String("dBm");

        lcd.printLine(1, info);

        unsigned long startTime = millis();
        while (millis() - startTime < 5000) {  // 3 segundos por red
            lcd.updateScrolling();
            delay(100);  // refresco del scroll
        }

        lcd.stopScrolling();
    }
}

void loop() {
    // No hacemos nada más aquí de momento
}
