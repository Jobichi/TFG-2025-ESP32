#include "Screens/OledManager.h"
#include "Sensors/Ds18b20Sensor.h"

Ds18b20Sensor sensorSalon(4, 2000, "Salón");   // GPIO4
Ds18b20Sensor sensorCocina(5, 2000, "Cocina"); // GPIO5

void setup() {
  Serial.begin(115200);
  setupOLED();

  sensorSalon.begin();
  sensorCocina.begin();
}

void loop() {
  sensorSalon.loop();
  sensorCocina.loop();

  String linea1 = String(sensorSalon.name()) + ": " + String(sensorSalon.lastTemperatureC(), 1) + "°C";
  String linea2 = String(sensorCocina.name()) + ": " + String(sensorCocina.lastTemperatureC(), 1) + "°C";

  oledSetTopText(linea1, 1);
  oledSetBottomText(linea2, 1);
  oledHandle();
}
        