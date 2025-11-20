#include <Arduino.h>
#include <Connectivity/wifi/WifiManager.h>
#include <Connectivity/mqtt/MqttManager.h>
#include <Connectivity/mqtt/Dispatcher.h>

#include <Connectivity/mqtt/handlers/getHandler.h>
#include <Connectivity/mqtt/handlers/setHandler.h>

#include <Connectivity/mqtt/publishers/AnnouncePublisher.h>
#include <Connectivity/mqtt/publishers/UpdatePublisher.h>
#include <Connectivity/mqtt/publishers/AlertPublisher.h>

// ==== Sensores simulados ====
#include <Devices/Sensors/FireSensor.h>
#include <Devices/Sensors/WaterLeakSensor.h>
#include <Devices/Sensors/DoorSensor.h>
#include <Devices/Sensors/LdrSensor.h>
#include <Devices/Sensors/MqSensor.h>
#include <Devices/Sensors/PirSensor.h>

// ==== Actuadores simulados ====
#include <Devices/Actuators/Buzzer.h>
#include <Devices/Actuators/Relay.h>
#include <Devices/Actuators/ServoMotor360.h>

// ===============================
//  Configuración principal
// ===============================

WifiManager wifiManager;
MqttManager mqttManager(wifiManager);

// Diccionarios globales
std::map<int, Sensor*> sensors;
std::map<int, Actuator*> actuators;

// Módulos MQTT
Dispatcher* dispatcher = nullptr;
AnnouncePublisher* announcePublisher = nullptr;
UpdatePublisher* updatePublisher = nullptr;
AlertPublisher* alertPublisher = nullptr;

// ===============================
//        Setup
// ===============================
void setup() {
    Serial.begin(115200);
    delay(500);

    Serial.println("\n=== TFG 2025 · ESP32 · MQTT Test Mode ===");

    // ---- WiFi ----
    wifiManager.begin();

    // ---- MQTT ----
    mqttManager.begin();

    // ---- Crear sensores (SIMULADOS) ----
    sensors[0] = new FireSensor(26);         // GPIO 26
    sensors[1] = new WaterLeakSensor(27);    // GPIO 27
    sensors[2] = new DoorSensor(14);         // GPIO 14
    sensors[3] = new LdrSensor(34);          // ADC 34
    sensors[4] = new MqSensor(35);           // ADC 35
    sensors[5] = new PirSensor(25);          // GPIO 25

    for (auto &s : sensors) {
        s.second->begin();
    }

    // ---- Crear actuadores (SIMULADOS) ----
    actuators[0] = new Buzzer(12);                // GPIO 12
    actuators[1] = new Relay(13);                 // GPIO 13
    actuators[2] = new ServoMotor360(23);         // GPIO 23

    for (auto &a : actuators)
        a.second->begin();

    // ---- Dispatcher ----
    dispatcher = new Dispatcher(sensors, actuators, mqttManager);

    // ---- Handlers ----
    dispatcher->registerHandler("get",
    [&](const HandlerContext &ctx){ GetHandler().handle(ctx); }
    );

    dispatcher->registerHandler("set",
        [&](const HandlerContext &ctx){ SetHandler().handle(ctx); }
    );

    // ---- Conectar dispatcher al MQTT Manager ----
    mqttManager.addSubHandler(
        "get/#",
        [&](const String &topic, const String &payload){ dispatcher->route(topic, payload); }
    );

    mqttManager.addSubHandler(
        "set/#",
        [&](const String &topic, const String &payload){ dispatcher->route(topic, payload); }
    );

    // ---- Publishers ----
    announcePublisher = new AnnouncePublisher(sensors, actuators, mqttManager);
    updatePublisher   = new UpdatePublisher(sensors, actuators, mqttManager);
    alertPublisher = new AlertPublisher(sensors, actuators, mqttManager);

    // ---- Publicamos "announce" para ver toda la estructura MQTT ----
    Serial.println("[MAIN] Publicando announce inicial...");
    announcePublisher->publishAll();
}

// ===============================
//               Loop
// ===============================

unsigned long lastUpdate = 0;
unsigned long lastAlert = 0;

void loop() {
    wifiManager.loop();
    mqttManager.loop();

    // ---- Publicación periódica de UPDATE ----
    if (millis() - lastUpdate >= 8000) {   // Cada 8 segundos
        lastUpdate = millis();

        Serial.println("[MAIN] Enviando update de sensores/actuadores...");
        updatePublisher->publishAll();
    }

    // ---- Alertas simuladas ----
    if (millis() - lastAlert >= 15000) {   // Cada 15 segundos
        lastAlert = millis();

        Serial.println("[MAIN] Publicando alerta simulada...");

        alertPublisher->publishSensorAlert(
            0,                            // sensor ID
            "warning",                    // nivel
            "Alerta simulada de fuego", // mensaje
            101                           // código
        );
    }

    // ---- Simulación de datos en sensores ----
    // No hay hardware conectado → forzamos valores manualmente

    // LDR → alterna entre 20% y 80%
    static bool ldrFlag = false;
    LdrSensor* ldr = (LdrSensor*) sensors[3];
    if (ldr) {
        ldr->setEnabled(true); // solo por si acaso
        if (ldrFlag) ldr->simulateValue(20);
        else         ldr->simulateValue(80);
        ldrFlag = !ldrFlag;
    }

    // MQ → alterna entre normal y humo
    static bool mqFlag = false;
    MqSensor* mq = (MqSensor*) sensors[4];
    if (mq) {
        if (mqFlag) mq->simulateVoltage(0.5);
        else        mq->simulateVoltage(2.8);
        mqFlag = !mqFlag;
    }

    // PIR → parpadeo de movimiento
    PirSensor* pir = (PirSensor*) sensors[5];
    if (pir) {
        pir->simulateState(random(0, 2));  // 0 o 1
    }

    // Fuego → se activa de vez en cuando
    FireSensor* fire = (FireSensor*) sensors[0];
    if (fire) {
        fire->simulateState(random(0, 10) > 7);  // 20% probabilidad
    }

    // Agua → probabilidad baja
    WaterLeakSensor* water = (WaterLeakSensor*) sensors[1];
    if (water) {
        water->simulateState(random(0, 20) == 0); // 5% probabilidad
    }

    // Puerta → cambia cada X loops
    DoorSensor* door = (DoorSensor*) sensors[2];
    if (door) {
        door->simulateState(random(0, 100) > 95); // 5% prob
    }
}