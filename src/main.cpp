// src/main.cpp
#include <Arduino.h>
#include <Logic/NodeApp.h>

// Si tus includes no van por NodeApp/DeviceRegistry, inclúyelos aquí también.
// (Con la modularización que te pasé, DeviceRegistry ya incluye los headers de sensores/actuadores.)

NodeApp app;

// -------------------------
// Simulación de entradas (solo modo test sin HW)
// -------------------------
static void simulateInputs(DeviceRegistry& dev) {
    std::map<int, Sensor*>& s = dev.sensors();

    // LDR (id=3): alterna 20/80 cada ~1s
    static uint32_t lastLdrMs = 0;
    static bool ldrFlag = false;
    if (millis() - lastLdrMs >= 1000) {
        lastLdrMs = millis();
        if (s.count(3)) {
            LdrSensor* ldr = static_cast<LdrSensor*>(s[3]);
            if (ldr) {
                ldr->simulateValue(ldrFlag ? 20 : 80);
                ldrFlag = !ldrFlag;
            }
        }
    }

    // MQ (id=4): alterna normal (0.5V) / humo (2.9V) cada ~6s
    static uint32_t lastMqMs = 0;
    static bool mqSmoke = false;
    if (millis() - lastMqMs >= 6000) {
        lastMqMs = millis();
        mqSmoke = !mqSmoke;
        if (s.count(4)) {
            MqSensor* mq = static_cast<MqSensor*>(s[4]);
            if (mq) {
                mq->simulateVoltage(mqSmoke ? 2.9f : 0.5f);
            }
        }
    }

    // PIR (id=5): pulso realista (ON 2s cada 10s)
    static uint32_t pirWindowStart = 0;
    static bool pirOn = false;

    if (pirWindowStart == 0) pirWindowStart = millis();

    const uint32_t elapsed = millis() - pirWindowStart;
    if (elapsed >= 10000) {
        pirWindowStart = millis();
    }

    const bool shouldBeOn = (elapsed < 2000);
    if (shouldBeOn != pirOn) {
        pirOn = shouldBeOn;
        if (s.count(5)) {
            PirSensor* pir = static_cast<PirSensor*>(s[5]);
            if (pir) {
                pir->simulateState(pirOn ? 1 : 0);
            }
        }
    }

    // Fuego (id=0): evento raro (ON 3s cada ~30s aprox)
    static uint32_t fireT0 = 0;
    static bool fireOn = false;
    if (fireT0 == 0) fireT0 = millis();

    if (!fireOn && (millis() - fireT0) > 30000) {
        fireOn = true;
        fireT0 = millis();
        if (s.count(0)) {
            FireSensor* fire = static_cast<FireSensor*>(s[0]);
            if (fire) fire->simulateState(true);
        }
    } else if (fireOn && (millis() - fireT0) > 3000) {
        fireOn = false;
        fireT0 = millis();
        if (s.count(0)) {
            FireSensor* fire = static_cast<FireSensor*>(s[0]);
            if (fire) fire->simulateState(false);
        }
    }

    // Agua (id=1): evento muy raro (ON 4s cada ~45s aprox)
    static uint32_t waterT0 = 0;
    static bool waterOn = false;
    if (waterT0 == 0) waterT0 = millis();

    if (!waterOn && (millis() - waterT0) > 45000) {
        waterOn = true;
        waterT0 = millis();
        if (s.count(1)) {
            WaterLeakSensor* w = static_cast<WaterLeakSensor*>(s[1]);
            if (w) w->simulateState(true);
        }
    } else if (waterOn && (millis() - waterT0) > 4000) {
        waterOn = false;
        waterT0 = millis();
        if (s.count(1)) {
            WaterLeakSensor* w = static_cast<WaterLeakSensor*>(s[1]);
            if (w) w->simulateState(false);
        }
    }

    // Puerta (id=2): cambia cada ~12s
    static uint32_t lastDoorMs = 0;
    static bool doorOpen = false;
    if (millis() - lastDoorMs >= 12000) {
        lastDoorMs = millis();
        doorOpen = !doorOpen;
        if (s.count(2)) {
            DoorSensor* d = static_cast<DoorSensor*>(s[2]);
            if (d) d->simulateState(doorOpen ? 1 : 0);
        }
    }
}

void setup() {

}

void loop() {

}
