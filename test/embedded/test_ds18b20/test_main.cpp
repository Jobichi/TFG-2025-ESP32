#include <Arduino.h>
#include <unity.h>

#include <Devices/Sensors/Ds18b20Sensor.h>

static constexpr uint8_t DS_PIN = 4;                 // AJUSTA a tu pin real
static constexpr unsigned long READ_PERIOD_MS = 2000; // DS18B20 típico

static Ds18b20Sensor* dsPtr = nullptr;

static void pollSensor(unsigned long totalMs, unsigned long stepMs = 100) {
    const unsigned long start = millis();
    while (millis() - start < totalMs) {
        dsPtr->loop();
        delay(stepMs);
    }
}

void test_ds18b20_begin_ok() {
    TEST_ASSERT_NOT_NULL(dsPtr);

    const bool ok = dsPtr->begin();
    Serial.printf("[DS18B20] begin() => %d\n", (int)ok);
    Serial.flush();

    TEST_ASSERT_TRUE_MESSAGE(ok, "DS18B20 no detectado o lectura invalida (revisa cableado/pull-up)");
}

void test_ds18b20_units_and_enabled() {
    TEST_ASSERT_NOT_NULL(dsPtr);

    TEST_ASSERT_TRUE(dsPtr->isEnabled());
    TEST_ASSERT_EQUAL_STRING("°C", dsPtr->getUnits());
}

void test_ds18b20_loop_updates_value_and_health() {
    TEST_ASSERT_NOT_NULL(dsPtr);

    // Espera varias ventanas de lectura por si la primera tarda
    pollSensor(READ_PERIOD_MS * 4, 150);

    const float t = dsPtr->getValue();
    const bool healthy = dsPtr->isHealthy();

    Serial.printf("[DS18B20] healthy=%d, T=%.2f C\n", (int)healthy, t);
    Serial.flush();

    // Validaciones básicas
    TEST_ASSERT_TRUE_MESSAGE(healthy, "Sensor no healthy tras loop()");
    TEST_ASSERT_FALSE_MESSAGE(isnan(t), "Temperatura NaN");

    // Rango razonable para DS18B20 (-55..125)
    TEST_ASSERT_TRUE_MESSAGE(t > -60.0f && t < 130.0f, "Temperatura fuera de rango esperado DS18B20");

    // Extra: detectar el caso típico de desconexión (-127 o 85)
    // 85°C puede aparecer como valor por defecto tras power-up si no hay conversión correcta.
    TEST_ASSERT_TRUE_MESSAGE(t != -127.0f, "DEVICE_DISCONNECTED_C tipico (-127): sensor desconectado?");
}

void test_ds18b20_disable_stops_updates() {
    TEST_ASSERT_NOT_NULL(dsPtr);

    // Asegura que hay una lectura inicial
    pollSensor(READ_PERIOD_MS * 2, 150);
    const float before = dsPtr->getValue();
    TEST_ASSERT_FALSE(isnan(before));

    dsPtr->setEnabled(false);

    // Espera mas que el periodo y llama loop: no debería actualizar
    delay(READ_PERIOD_MS + 300);
    dsPtr->loop();

    const float after = dsPtr->getValue();

    Serial.printf("[DS18B20] disabled: before=%.2f after=%.2f\n", before, after);
    Serial.flush();

    TEST_ASSERT_FLOAT_WITHIN(0.01f, before, after);

    dsPtr->setEnabled(true);
}

void setup() {
    delay(1500);
    Serial.begin(115200);
    delay(200);

    Serial.println("[TEST] DS18B20 embedded tests");
    Serial.printf("[DS18B20] pin=%u period=%lu ms\n", (unsigned)DS_PIN, READ_PERIOD_MS);
    Serial.flush();

    Ds18b20Config cfg;
    cfg.readPeriodMs = READ_PERIOD_MS;
    cfg.friendlyName = "Ds18b20Test";
    cfg.location = "mesa";

    static Ds18b20Sensor ds(DS_PIN, cfg);
    dsPtr = &ds;

    UNITY_BEGIN();
    RUN_TEST(test_ds18b20_begin_ok);
    RUN_TEST(test_ds18b20_units_and_enabled);
    RUN_TEST(test_ds18b20_loop_updates_value_and_health);
    RUN_TEST(test_ds18b20_disable_stops_updates);
    const int fails = UNITY_END();

    Serial.printf("[TEST] DS18B20 finished. fails=%d\n", fails);
    Serial.flush();

    while (true) delay(1000);
}

void loop() {}
