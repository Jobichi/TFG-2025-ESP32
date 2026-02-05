#include <Arduino.h>
#include <Logic/NodeApp.h>

// MQTT Updates messages
static constexpr uint32_t UPDATE_PERIOD_MS = 8000;

// Pines (AJUSTA A TU MONTAJE)
static constexpr uint8_t PIN_MQ2 = 34;      // ADC1 recomendado (GPIO34/35/32/33...)
static constexpr uint8_t PIN_DS18B20 = 4;   // 1-Wire
static constexpr uint8_t PIN_BUZZER = 27;   // Salida digital

// IDs - Para BBDD y Publicaciones/Subscripciones
static constexpr int SID_MQ2 = 0;
static constexpr int SID_DS18B20 = 1;

static constexpr int AID_BUZZER = 0;

// MQ2 - Umbral en voltios para considerar "gas detectado" (calibra según tu módulo)
static constexpr float MQ2_THRESHOLD_V = 0.3f;

// Códigos de alerta (estables para tu router/BBDD si los usas)
static constexpr int ALERT_GAS_ON_CODE  = 100;
static constexpr int ALERT_GAS_OFF_CODE = 101;

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
            800UL,              // periodo lectura
            MQ2_THRESHOLD_V      // umbral para triggered/alertas
        );

        // Sensor DS18B20 (temperatura)
        dev.addDs18b20(
            SID_DS18B20,
            PIN_DS18B20,
            "Sensor de Temperatura",
            "cocina",
            2000UL
        );

        // Actuador Buzzer
        dev.addBuzzer(
            AID_BUZZER,
            PIN_BUZZER,
            "Alarma",
            "cocina"
        );
    };

    auto alertSetup = [](AlertEngine& ae) {
        // Publica alerta ON cuando MQ2 >= umbral, y OFF cuando baja
        ae.addBooleanChange(
            SID_MQ2,
            MQ2_THRESHOLD_V,
            "critical", "GAS_DETECTED", ALERT_GAS_ON_CODE,
            "info",     "GAS_CLEARED",  ALERT_GAS_OFF_CODE
        );
    };

    app.begin(deviceSetup, opt, alertSetup);

    Serial.println("[BOOT] Inicializada demo cocina: Buzzer + MQ2 + DS18B20");
    Serial.flush();
}

void loop() {
    app.loop();

    DeviceRegistry& dev = app.devices();

    Sensor* mq2 = getSensor(dev, SID_MQ2);
    Actuator* buzzer = getActuator(dev, AID_BUZZER);

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
