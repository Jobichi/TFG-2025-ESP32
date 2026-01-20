#include <Arduino.h>
#include <Logic/NodeApp.h>

// MQTT Updates messages
static constexpr uint32_t UPDATE_PERIOD_MS = 8000;

// Pines 
static constexpr uint8_t PIN_DHT      = 4; // ADC1

static constexpr uint8_t PIN_LED   = 13;
static constexpr uint8_t PIN_FEEDBACK_LED = 27;

// IDs - Para BBDD y Publicaciones/Subscripciones

static constexpr int SID_DHT_H  = 0;
static constexpr int SID_DHT_T  = 1;

static constexpr int AID_LED = 1;

NodeApp app;

void setup() {
    delay(800);
    Serial.begin(115200);
    delay(200);

    NodeApp::Options opt;
    opt.announceOnBoot = true;
    opt.periodicUpdates = true;
    opt.updatePeriodMs = UPDATE_PERIOD_MS;

    opt.waitWifi = true;   // entorno real: espera conectividad
    opt.waitMqtt = true;

    auto deviceSetup = [](DeviceRegistry& dev) {
        // Sensores
        dev.addDht22Pair(SID_DHT_T, SID_DHT_H, PIN_DHT, "habitación", 2000UL, "Sensor de Temperatura", "Sensor de Humedad");


        // Actuadores
        dev.addBuzzerWithFeedback(AID_LED, PIN_LED, PIN_FEEDBACK_LED, "Luz", "habitación", true, 40, true);
    };

    auto alertSetup = [](AlertEngine& ae) {
        // Ejemplos (ajusta a tu gusto)
        // ae.addBooleanChange(SID_PIR, 0.5f,
        //                     "warning", "Movimiento detectado", 401,
        //                     "info", "Sin movimiento", 402);
    };

    app.begin(deviceSetup, opt, alertSetup);

    Serial.println("[BOOT] Inicializado el programa para el Manual de Usuario.");
    Serial.flush();
}

void loop() {
    app.loop();
    delay(5);
}
