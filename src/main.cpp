#include "networks/WiFiManager.hpp"

// Config inicial (DHCP o IP fija)
NetworkConfig cfg;
void setup() {
  Serial.begin(115200);

  cfg.ssid = "TuSSID";
  cfg.password = "TuPass";
  cfg.isDHCP = true;             // o false y rellenas staticIP/gateway/subnet
  cfg.hostname = "mi-dispositivo";
  cfg.mdnsEnabled = true;

  static WiFiManager wifi(cfg);
  wifi.setConnectTimeoutMs(10000);
  wifi.setBackoffBaseMs(1000);
  wifi.setBackoffMaxMs(30000);

  wifi.begin();                  // aplica config y lanza primer intento

  // Guarda una referencia global/estática si lo necesitas en loop()
}

void loop() {
  // asume acceso a 'wifi' (global/estático o vía singleton)
  extern WiFiManager wifi;

  wifi.tick();                   // gestiona FSM y reintentos

  if (wifi.isConnected()) {
    // Trabajo con conectividad
  } else {
    // Trabajo sin conectividad
  }

  // No uses delays largos; loop debe ser fluido
}
