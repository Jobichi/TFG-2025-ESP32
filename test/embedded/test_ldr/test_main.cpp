#include <Arduino.h>
#include <unity.h>
#include <math.h>

#include <Devices/Sensors/LdrSensor.h>

static constexpr uint8_t LDR_PIN = 34;              // ADC1 recomendado
static constexpr unsigned long READ_PERIOD_MS = 200;

static LdrSensor* ldrPtr = nullptr;

static void pollForUpdates(unsigned long totalMs, unsigned long stepMs = 50) {
    const unsigned long start = millis();
    while (millis() - start < totalMs) {
        ldrPtr->loop();
        delay(stepMs);
    }
}

// Welford para media y varianza (estable numéricamente)
struct Stats {
    uint32_t n{0};
    double mean{0.0};
    double m2{0.0};
    int minV{INT32_MAX};
    int maxV{INT32_MIN};

    void add(int x) {
        n++;
        if (x < minV) minV = x;
        if (x > maxV) maxV = x;

        const double dx = (double)x - mean;
        mean += dx / (double)n;
        const double dx2 = (double)x - mean;
        m2 += dx * dx2;
    }

    double variance() const {
        if (n < 2) return 0.0;
        return m2 / (double)(n - 1);
    }

    double stddev() const {
        return sqrt(variance());
    }

    int p2p() const {
        if (n == 0) return 0;
        return maxV - minV;
    }
};

void test_ldr_begin_ok() {
    TEST_ASSERT_NOT_NULL(ldrPtr);
    TEST_ASSERT_TRUE(ldrPtr->begin());
    TEST_ASSERT_TRUE(ldrPtr->isHealthy());
}

void test_ldr_units_and_enabled() {
    TEST_ASSERT_NOT_NULL(ldrPtr);
    TEST_ASSERT_TRUE(ldrPtr->isEnabled());
    TEST_ASSERT_EQUAL_STRING("%", ldrPtr->getUnits());
}

void test_ldr_loop_updates_and_range() {
    TEST_ASSERT_NOT_NULL(ldrPtr);

    pollForUpdates(READ_PERIOD_MS * 6, 60);

    const int raw = ldrPtr->lastRaw();
    const float pct = ldrPtr->getValue();

    Serial.printf("[LDR] class raw=%d pct=%.2f%% healthy=%d\n", raw, pct, (int)ldrPtr->isHealthy());
    Serial.flush();

    TEST_ASSERT_TRUE_MESSAGE(ldrPtr->isHealthy(), "LDR no healthy tras loop()");
    TEST_ASSERT_TRUE_MESSAGE(pct >= 0.0f && pct <= 100.0f, "Porcentaje fuera de [0,100]");
    TEST_ASSERT_TRUE_MESSAGE(raw >= 0 && raw <= 4095, "ADC raw fuera de [0,4095]");
}

void test_ldr_disable_stops_updates() {
    TEST_ASSERT_NOT_NULL(ldrPtr);

    pollForUpdates(READ_PERIOD_MS * 4, 60);
    const int rawBefore = ldrPtr->lastRaw();
    const float pctBefore = ldrPtr->getValue();

    ldrPtr->setEnabled(false);

    delay(READ_PERIOD_MS + 100);
    ldrPtr->loop();

    const int rawAfter = ldrPtr->lastRaw();
    const float pctAfter = ldrPtr->getValue();

    Serial.printf("[LDR] disabled: raw %d->%d pct %.2f->%.2f\n", rawBefore, rawAfter, pctBefore, pctAfter);
    Serial.flush();

    TEST_ASSERT_EQUAL_INT(rawBefore, rawAfter);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, pctBefore, pctAfter);

    ldrPtr->setEnabled(true);
}

void test_ldr_simulate_value_sets_percent() {
    TEST_ASSERT_NOT_NULL(ldrPtr);

    ldrPtr->simulateValue(42);
    TEST_ASSERT_EQUAL_INT(42, (int)ldrPtr->getValue());
}

// Test diagnóstico para ver el efecto del condensador (ruido)
// Ejecuta esto con una luz constante (sin mover nada) y compara resultados:
// - sin condensador
// - con condensador
void test_ldr_adc_noise_stats() {
    // Ajusta ventana y periodo de muestreo para ver bien el jitter
    static constexpr uint32_t SAMPLE_MS = 3000;
    static constexpr uint32_t STEP_MS = 10; // 100 Hz aprox

    Stats st;

    // Pequeña estabilización
    delay(300);

    const uint32_t start = millis();
    while (millis() - start < SAMPLE_MS) {
        const int raw = analogRead(LDR_PIN);
        st.add(raw);
        delay(STEP_MS);
    }

    Serial.printf("[LDR] ADC noise stats (%lu ms @ %lu ms): n=%lu min=%d max=%d p2p=%d mean=%.1f std=%.2f\n",
                  (unsigned long)SAMPLE_MS,
                  (unsigned long)STEP_MS,
                  (unsigned long)st.n,
                  st.minV, st.maxV, st.p2p(),
                  st.mean, st.stddev());
    Serial.flush();

    // No pongo asserts estrictos porque dependen mucho del montaje.
    // Si quieres forzarlo, puedes descomentar y ajustar el umbral:
    // TEST_ASSERT_TRUE_MESSAGE(st.p2p() < 80, "Demasiado ruido (p2p alto). Prueba condensador o filtro software");
}

void setup() {
    delay(1500);
    Serial.begin(115200);
    delay(200);

    Serial.println("[TEST] LDR embedded tests");
    Serial.printf("[LDR] pin=%u period=%lu ms\n", (unsigned)LDR_PIN, (unsigned long)READ_PERIOD_MS);
    Serial.flush();

    // Opcional: fija resolución ADC si quieres asegurarte
    // analogReadResolution(12);

    LdrConfig cfg;
    cfg.readPeriodMs = READ_PERIOD_MS;
    cfg.friendlyName = "LdrTest";
    cfg.location = "mesa";

    static LdrSensor ldr(LDR_PIN, cfg);
    ldrPtr = &ldr;

    UNITY_BEGIN();
    RUN_TEST(test_ldr_begin_ok);
    RUN_TEST(test_ldr_units_and_enabled);
    RUN_TEST(test_ldr_loop_updates_and_range);
    RUN_TEST(test_ldr_disable_stops_updates);
    RUN_TEST(test_ldr_simulate_value_sets_percent);

    // Diagnóstico del filtro hardware
    RUN_TEST(test_ldr_adc_noise_stats);

    const int fails = UNITY_END();

    Serial.printf("[TEST] LDR finished. fails=%d\n", fails);
    Serial.flush();

    while (true) delay(1000);
}

void loop() {}
