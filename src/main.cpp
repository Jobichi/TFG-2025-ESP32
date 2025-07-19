#include <Arduino.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();
  lcd.backlight(); // Asegura iluminación
  lcd.setCursor(0, 0);
  lcd.print("Prueba LCD");
}

void loop() {}
