#include <Arduino.h>
#include <unity.h>

#include <Devices/Sensors/PirSensor.h>

static constexpr uint8_t PIR_PIN = 27; // AJUSTA al pin real

static PirSensor* pirPtr = nullptr;

static void runLoopFor(unsigned long totalMs, unsigned long stepMs = 10) {
    const unsigned long start = millis();
    while (millis() - start < totalMs) {
        pirPtr->loop();
        delay(stepMs);
    }
}

void test_pir_begin_ok() {
    TEST_ASSERT_NOT_NULL(pirPtr);
    TEST_ASSERT_TRUE(pirPtr->begin());
    TEST_ASSERT_TRUE(pirPtr->isHealthy());
    TEST_ASSERT_TRUE(pirPtr->isEnabled());
    TEST_ASSERT_EQUAL_FLOAT(0.0f, pirPtr->getValue());
}

void test_pir_warmup_blocks_changes() {
    TEST_ASSERT_NOT_NULL(pirPtr);

    // Durante warmup, aunque el pin fluctúe (si está flotante), loop no debe actualizar
    const unsigned long t0 = pirPtr->lastChangeMs();
    const bool m0 = pirPtr->motion();

    runLoopFor(300); // menos que warmup (lo ponemos corto en setup)

    TEST_ASSERT_EQUAL_UINT32(t0, pirPtr->lastChangeMs());
    TEST_ASSERT_EQUAL(m0, pirPtr->motion());
}

void test_pir_disable_stops_processing() {
    TEST_ASSERT_NOT_NULL(pirPtr);

    pirPtr->setEnabled(false);
    const unsigned long t0 = pirPtr->lastChangeMs();
    runLoopFor(300);
    TEST_ASSERT_EQUAL_UINT32(t0, pirPtr->lastChangeMs());
    pirPtr->setEnabled(true);
}

// Test “manual”: con warmup ya pasado, cambia el nivel del pin a HIGH y mantenlo > stableMs
// Si tienes PIR real, muévete delante. Si no, puentea PIR_PIN a 3.3V/GND.
void test_pir_debounce_and_state_change_manual() {
    TEST_ASSERT_NOT_NULL(pirPtr);

    // Espera a superar warmup
    runLoopFor(700); // warmup será 500ms en setup

    Serial.println("[PIR] MANUAL: fuerza el pin a estado ACTIVO y mantenlo > stableMs (200ms).");
    Serial.flush();

    // Damos una ventana para que hagas el cambio
    runLoopFor(1500);

    // No hacemos assert estricto porque depende del hardware/acción manual.
    // Solo imprimimos estado final.
    Serial.printf("[PIR] motion=%d value=%.1f lastChangeMs=%lu\n",
                  (int)pirPtr->motion(),
                  pirPtr->getValue(),
                  (unsigned long)pirPtr->lastChangeMs());
    Serial.flush();

    TEST_PASS();
}

void setup() {
    delay(1500);
    Serial.begin(115200);
    delay(200);

    Serial.println("[TEST] PIR embedded tests");
    Serial.printf("[PIR] pin=%u\n", (unsigned)PIR_PIN);
    Serial.flush();

    PirConfig cfg;
    cfg.activeHigh = true;
    cfg.usePulldown = false;   // si el pin flota, usa PULLDOWN externo o INPUT_PULLDOWN si lo soporta tu placa
    cfg.warmupMs = 500;        // corto para test
    cfg.stableMs = 200;        // más fácil de observar

    static PirSensor pir(PIR_PIN, cfg);
    pirPtr = &pir;

    UNITY_BEGIN();
    RUN_TEST(test_pir_begin_ok);
    RUN_TEST(test_pir_warmup_blocks_changes);
    RUN_TEST(test_pir_disable_stops_processing);
    RUN_TEST(test_pir_debounce_and_state_change_manual);
    const int fails = UNITY_END();

    Serial.printf("[TEST] PIR finished. fails=%d\n", fails);
    Serial.flush();

    while (true) delay(1000);
}

void loop() {}
