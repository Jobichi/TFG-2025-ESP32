#include <Arduino.h>
#include <Logic/NodeApp.h>

// MQTT Updates messages
static constexpr uint32_t UPDATE_PERIOD_MS = 8000;

// Pines 
static constexpr uint8_t PIN_DHT      = 4;
static constexpr uint8_t PIN_DS18B20  = 5;
static constexpr uint8_t PIN_LDR      = 34; // ADC1
static constexpr uint8_t PIN_MQ       = 35; // ADC1
static constexpr uint8_t PIN_PIR      = 27;

static constexpr uint8_t PIN_RELAY    = 23;
static constexpr uint8_t PIN_BUZZER   = 13;
static constexpr uint8_t PIN_SERVO    = 18;

// IDs - Para BBDD y Publicaciones/Subscripciones
static constexpr int SID_BMP280 = 0;
static constexpr int SID_DHT_H  = 1;
static constexpr int SID_DHT_T  = 2;
static constexpr int SID_DS18   = 3;
static constexpr int SID_LDR    = 4;
static constexpr int SID_MQ     = 5;
static constexpr int SID_PIR    = 6;

static constexpr int AID_RELAY  = 0;
static constexpr int AID_BUZZER = 1;
static constexpr int AID_SERVO  = 2;

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
        dev.addBmp280(SID_BMP280, "Sensor de presión", "salon", 0x76, 2000UL);

        dev.addDht22Pair(SID_DHT_T, SID_DHT_H, PIN_DHT, "salon", 2000UL, "Humedad", "Temperatura");
        // Si en tu registry no existe addDht22 doble, entonces:
        // dev.addDhtHumidity(SID_DHT_H, PIN_DHT, 2000UL, ...);
        // dev.addDhtTemperature(SID_DHT_T, PIN_DHT, 2000UL, ...);

        dev.addDs18b20(SID_DS18, PIN_DS18B20, "Sensor de temperatura", "salon", 2000UL);
        dev.addLdr(SID_LDR, PIN_LDR, "Sensor de luz", "salon", 200UL);

        dev.addMq(SID_MQ, PIN_MQ, "Sensor de gas", "cocina", 1000UL, 2.0f);   // thresholdV ajusta

        dev.addPir(SID_PIR, PIN_PIR, "PIR", "pasillo",
                   /*activeHigh*/true, /*usePulldown*/false, /*warmup*/60000UL, /*stable*/80UL);

        // Actuadores
        dev.addRelay(AID_RELAY, PIN_RELAY, "Luz principal", "salon", /*activeLow*/true);
        dev.addBuzzer(AID_BUZZER, PIN_BUZZER, "Alarma", "salon");
        dev.addServo360(AID_SERVO, PIN_SERVO, "Persianas", "salon", /*channelHint*/0);
    };

    auto alertSetup = [](AlertEngine& ae) {
        // Ejemplos (ajusta a tu gusto)
        ae.addBooleanChange(SID_PIR, 0.5f,
                            "warning", "Movimiento detectado", 401,
                            "info", "Sin movimiento", 402);

        ae.addHysteresis(SID_LDR, 20.0f, 35.0f,
                         "warning", "Ambiente oscuro", 601,
                         "info", "Luz normal", 602);

        ae.addHysteresis(SID_MQ, 1.7f, 2.1f,
                         "warning", "Posible humo/gas (MQ)", 701,
                         "info", "MQ normal", 702);
    };

    app.begin(deviceSetup, opt, alertSetup);

    Serial.println("[BOOT] NodeApp real iniciado.");
    Serial.flush();
}

void loop() {
    app.loop();
    delay(5);
}
