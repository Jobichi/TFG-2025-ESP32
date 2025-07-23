#include <Arduino.h>
#include <WiFiManager/WiFiManager.h>
#include <LCDscreen.h>

LCDScreen lcd;
WifiManager wifi("R-73", "20924273", 2, 30000, 3, &lcd);  // LED en pin 2, pantalla incluida

const int ledPinD4 = 4;
const int buttonPinD5 = 5;
bool ledState = false;
bool lastButtonState = HIGH;

void setup() {
    Serial.begin(115200);
    pinMode(buttonPinD5, INPUT_PULLDOWN);
    pinMode(ledPinD4, OUTPUT);
    digitalWrite(ledPinD4, ledState);
    lcd.begin();
    wifi.connect();  // Ya muestra estado en la pantalla
}

void loop() {
    lcd.updateScrolling();
    wifi.checkConnection();  // Reconecta si se cae, mostrando estado
    
    bool buttonState = digitalRead(buttonPinD5);
    if(buttonState == LOW && lastButtonState == HIGH){
        ledState = !ledState;
        digitalWrite(ledPinD4, ledState);
        Serial.println(ledState ? "LED encendido" : "LED apagado");
        delay(10);
    }

    lastButtonState = buttonState;
}
