#include <Arduino.h>
#include <Logic/NodeApp.h>

// MQTT Updates messages
static constexpr uint32_t UPDATE_PERIOD_MS = 8000;

// Pines
static constexpr uint8_t PIN_DHT = 4; // ADC1

static constexpr uint8_t PIN_LED = 13;
static constexpr uint8_t PIN_FEEDBACK_LED = 27;

// --- Servo 360 + finales de carrera (ejemplo) ---
static constexpr uint8_t PIN_SERVO = 18;        // PWM (GPIO cualquiera válido)
static constexpr uint8_t PIN_END_OPEN = 32;     // Pulsador fin abierto (a GND, INPUT_PULLUP)
static constexpr uint8_t PIN_END_CLOSE = 33;    // Pulsador fin cerrado (a GND, INPUT_PULLUP)

// IDs - Para BBDD y Publicaciones/Subscripciones
static constexpr int SID_DHT_H = 0;
static constexpr int SID_DHT_T = 1;

static constexpr int AID_LED = 1;
static constexpr int AID_SERVO = 2;

NodeApp app;

void setup() {
    delay(800);
    Serial.begin(115200);
    delay(200);

    NodeApp::Options opt;
    opt.announceOnBoot = true;
    opt.periodicUpdates = true;
    opt.updatePeriodMs = UPDATE_PERIOD_MS;

    opt.waitWifi = true;
    opt.waitMqtt = true;

    auto deviceSetup = [](DeviceRegistry& dev) {
        // Sensores
        dev.addDht22Pair(
            SID_DHT_T, SID_DHT_H,
            PIN_DHT,
            "salon",
            2000UL,
            "Sensor de Temperatura",
            "Sensor de Humedad"
        );

        // Actuadores
        dev.addBuzzerWithFeedback(
            AID_LED,
            PIN_LED,
            PIN_FEEDBACK_LED,
            "Luz",
            "salon",
            true,
            40,
            true
        );

        // Servo 360 con finales de carrera (simulación persiana/puerta)
        // - activeLow=true asume INPUT_PULLUP (pulsado=LOW) en los finales
        // - debounceMs=30 para antirrebote
        // - maxRunMs=15000 por seguridad (timeout)
        dev.addServo360Endstops(
            AID_SERVO,
            PIN_SERVO,
            PIN_END_OPEN,
            PIN_END_CLOSE,
            "Persiana",
            "salon",
            0,        // channelHint/timer (0..3) según tu implementación
            true,     // activeLow
            30,       // debounceMs
            15000     // maxRunMs
        );
    };

    auto alertSetup = [](AlertEngine& ae) {
        // Ejemplos (ajusta a tu gusto)
    };

    app.begin(deviceSetup, opt, alertSetup);

    Serial.println("[BOOT] Inicializado demo salón: led + servo + dht22.");
    Serial.flush();
}

void loop() {
    app.loop();
    delay(5);
}
