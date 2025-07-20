#include <Arduino.h>
#include <WifiScanner.h>

void setup() {
    Serial.begin(115200);
    delay(1000);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();  // Importante para escaneo
    delay(100);

    WifiScanner::printNetworks();
}

void loop() {
   
}
