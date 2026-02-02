#include <Arduino.h>
#include <unity.h>

#include <Devices/Sensors/MqSensor.h>

// Usa ADC1 si vas a usar WiFi: p.ej. 32,33,34,35,36,39
static constexpr uint8_t MQ_PIN = 34;                 // AJUSTA a tu pin real
static constexpr unsigned long READ_PERIOD_MS = 200;  // corto para test

static MqSensor* mqPtr = nullptr;

static void pollForUpdates(unsigned long totalMs, unsigned long stepMs = 50) {
    const unsigned long start = millis();
    while (millis() - start < totalMs) {
        mqPtr->loop();
        delay(stepMs);
    }
}

void test_mq_begin_ok() {
    TEST_ASSERT_NOT_NULL(mqPtr);
    TEST_ASSERT_TRUE(mqPtr->begin());
    TEST_ASSERT_TRUE(mqPtr->isHealthy());
}

void test_mq_units_and_enabled() {
    TEST_ASSERT_NOT_NULL(mqPtr);
    TEST_ASSERT_TRUE(mqPtr->isEnabled());
    TEST_ASSERT_EQUAL_STRING("V", mqPtr->getUnits());
}

void test_mq_loop_updates_and_ranges() {
    TEST_ASSERT_NOT_NULL(mqPtr);

    pollForUpdates(READ_PERIOD_MS * 6, 60);

    const int raw = mqPtr->lastRaw();
    const float v = mqPtr->getValue();

    Serial.printf("[MQ] raw=%d v=%.3fV healthy=%d triggered=%d\n",
                  raw, v, (int)mqPtr->isHealthy(), (int)mqPtr->triggered());
    Serial.flush();

    TEST_ASSERT_TRUE_MESSAGE(mqPtr->isHealthy(), "MQ no healthy tras loop()");
    TEST_ASSERT_TRUE_MESSAGE(raw >= 0 && raw <= 4095, "ADC raw fuera de [0,4095]");
    TEST_ASSERT_TRUE_MESSAGE(v >= 0.0f && v <= 3.3f + 0.05f, "Voltaje fuera de [0,3.3]");
}

void test_mq_disable_stops_updates() {
    TEST_ASSERT_NOT_NULL(mqPtr);

    pollForUpdates(READ_PERIOD_MS * 4, 60);
    const int rawBefore = mqPtr->lastRaw();
    const float vBefore = mqPtr->getValue();
    const unsigned long tBefore = mqPtr->lastSampleMs();

    mqPtr->setEnabled(false);

    delay(READ_PERIOD_MS + 150);
    mqPtr->loop();

    const int rawAfter = mqPtr->lastRaw();
    const float vAfter = mqPtr->getValue();
    const unsigned long tAfter = mqPtr->lastSampleMs();

    Serial.printf("[MQ] disabled: raw %d->%d v %.3f->%.3f t %lu->%lu\n",
                  rawBefore, rawAfter, vBefore, vAfter, tBefore, tAfter);
    Serial.flush();

    TEST_ASSERT_EQUAL_INT(rawBefore, rawAfter);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, vBefore, vAfter);
    TEST_ASSERT_EQUAL_UINT32(tBefore, tAfter);

    mqPtr->setEnabled(true);
}

void test_mq_threshold_triggered_simulation() {
    // Umbral de prueba: 1.0V
    // (este test valida solo la lógica, no el ADC)
    TEST_ASSERT_NOT_NULL(mqPtr);

    mqPtr->simulateVoltage(0.5f);
    TEST_ASSERT_FALSE(mqPtr->triggered());

    mqPtr->simulateVoltage(1.2f);
    TEST_ASSERT_TRUE(mqPtr->triggered());
}

void setup() {
    delay(1500);
    Serial.begin(115200);
    delay(200);

    Serial.println("[TEST] MQ sensor embedded tests");
    Serial.printf("[MQ] pin=%u period=%lu ms\n", (unsigned)MQ_PIN, (unsigned long)READ_PERIOD_MS);
    Serial.flush();

    MqConfig cfg;
    cfg.readPeriodMs = READ_PERIOD_MS;
    cfg.thresholdVoltage = 1.0f;   // para test de triggered()
    cfg.friendlyName = "MqTest";
    cfg.location = "mesa";

    static MqSensor mq(MQ_PIN, cfg);
    mqPtr = &mq;

    UNITY_BEGIN();
    RUN_TEST(test_mq_begin_ok);
    RUN_TEST(test_mq_units_and_enabled);
    RUN_TEST(test_mq_loop_updates_and_ranges);
    RUN_TEST(test_mq_disable_stops_updates);
    RUN_TEST(test_mq_threshold_triggered_simulation);
    const int fails = UNITY_END();

    Serial.printf("[TEST] MQ finished. fails=%d\n", fails);
    Serial.flush();

    while (true) delay(1000);
}

void loop() {}
