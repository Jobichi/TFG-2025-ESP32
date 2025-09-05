#pragma once
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

void setupLCD();
void lcdPrint(const String& line1, const String& line2 ="");
void lcdClear();