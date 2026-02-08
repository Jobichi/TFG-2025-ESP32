#include <Arduino.h>
#include <Logic/NodeApp.h>

// MQTT Updates messages
static constexpr uint32_t UPDATE_PERIOD_MS = 8000;

// Pines (AJUSTA A TU MONTAJE)
static constexpr uint8_t PIN_MQ2 = 34;      // ADC1 recomendado (GPIO34/35/32/33...)
static constexpr uint8_t PIN_DHT = 4;       // DHT22 (dato digital)
static constexpr uint8_t PIN_BUZZER = 27;   // Salida digital

// IDs - Para BBDD y Publicaciones/Subscripciones
static constexpr int SID_MQ2   = 0;
static constexpr int SID_DHT_H = 1;
static constexpr int SID_DHT_T = 2;

static constexpr int AID_BUZZER = 0;

// MQ2 - Umbral en voltios para considerar "gas detectado" (calibra según tu módulo)
static constexpr float MQ2_THRESHOLD_V = 0.15f;

// Humedad - Umbral (%)
static constexpr float HUM_THRESHOLD_PCT = 90.0f;

// Códigos de alerta (estables para tu router/BBDD si los usas)
static constexpr int ALERT_GAS_ON_CODE   = 100;
static constexpr int ALERT_GAS_OFF_CODE  = 101;

static constexpr int ALERT_HUM_ON_CODE   = 200;
static constexpr int ALERT_HUM_OFF_CODE  = 201;

NodeApp app;

// Para evitar estar spameando comandos ON/OFF continuamente
static bool gasLatched = false;

static Sensor* getSensor(DeviceRegistry& dev, int id) {
    std::map<int, Sensor*>& smap = dev.sensors();
    std::map<int, Sensor*>::iterator it = smap.find(id);
    if (it == smap.end()) return nullptr;
    return it->second;
}

static Actuator* getActuator(DeviceRegistry& dev, int id) {
    std::map<int, Actuator*>& amap = dev.actuators();
    std::map<int, Actuator*>::iterator it = amap.find(id);
    if (it == amap.end()) return nullptr;
    return it->second;
}

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
        // Sensor MQ2 (analógico -> voltaje)
        dev.addMq(
            SID_MQ2,
            PIN_MQ2,
            "MQ-2 Gas",
            "cocina",
            800UL,
            MQ2_THRESHOLD_V
        );

        // Sensor DHT22 (temperatura + humedad)
        dev.addDht22Pair(
            SID_DHT_T, SID_DHT_H,
            PIN_DHT,
            "cocina",
            2000UL,
            "Sensor de Temperatura",
            "Sensor de Humedad"
        );

        // Actuador Buzzer (solo lo usaremos para GAS en loop())
        dev.addBuzzer(
            AID_BUZZER,
            PIN_BUZZER,
            "Alarma",
            "cocina"
        );
    };

    auto alertSetup = [](AlertEngine& ae) {
        // Gas: ON cuando MQ2 >= umbral, OFF cuando baja
        ae.addBooleanChange(
            SID_MQ2,
            MQ2_THRESHOLD_V,
            "critical", "GAS_DETECTED", ALERT_GAS_ON_CODE,
            "info",     "GAS_CLEARED",  ALERT_GAS_OFF_CODE
        );

        // Humedad: ON cuando H >= 90%, OFF cuando baja (NO activa buzzer)
        ae.addBooleanChange(
            SID_DHT_H,
            HUM_THRESHOLD_PCT,
            "warning",  "HUMIDITY_HIGH",   ALERT_HUM_ON_CODE,
            "info",     "HUMIDITY_NORMAL", ALERT_HUM_OFF_CODE
        );
    };

    app.begin(deviceSetup, opt, alertSetup);

    Serial.println("[BOOT] Inicializada demo cocina: Buzzer + MQ2 + DHT22");
    Serial.flush();
}

void loop() {
    app.loop();

    DeviceRegistry& dev = app.devices();

    Sensor* mq2 = getSensor(dev, SID_MQ2);
    Actuator* buzzer = getActuator(dev, AID_BUZZER);

    // Buzzer SOLO por gas
    if (mq2 && buzzer && mq2->isEnabled()) {
        const float mq2V = mq2->getValue();
        const bool gasNow = (mq2V >= MQ2_THRESHOLD_V);

        if (gasNow && !gasLatched) {
            buzzer->applyCommand("ON");
            gasLatched = true;
        } else if (!gasNow && gasLatched) {
            buzzer->applyCommand("OFF");
            gasLatched = false;
        }
    }

    delay(5);
}
 