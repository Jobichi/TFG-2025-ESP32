#include <Arduino.h>
#include <unity.h>

#include <Devices/Sensors/Dht22Base.h>
#include <Devices/Sensors/Dht22HumiditySensor.h>
#include <Devices/Sensors/Dht22TemperatureSensor.h>

static constexpr uint8_t DHT_PIN = 4;                // AJUSTA a tu pin real
static constexpr unsigned long PERIOD_MS = 2000UL;   // DHT22 suele requerir >= 2s

static Dht22Base* driverPtr = nullptr;
static Dht22HumiditySensor* humPtr = nullptr;
static Dht22TemperatureSensor* tempPtr = nullptr;

static void pollForRead(unsigned long totalMs, unsigned long stepMs = 100) {
    const unsigned long start = millis();
    while (millis() - start < totalMs) {
        humPtr->loop();
        tempPtr->loop();
        delay(stepMs);

        if (driverPtr->healthy()) {
            return;
        }
    }
}

void test_dht_begin_ok() {
    TEST_ASSERT_NOT_NULL(driverPtr);
    TEST_ASSERT_NOT_NULL(humPtr);
    TEST_ASSERT_NOT_NULL(tempPtr);

    TEST_ASSERT_TRUE(humPtr->begin());
    TEST_ASSERT_TRUE(tempPtr->begin());
}

void test_dht_eventually_reads_valid_values() {
    TEST_ASSERT_NOT_NULL(driverPtr);
    TEST_ASSERT_NOT_NULL(humPtr);
    TEST_ASSERT_NOT_NULL(tempPtr);

    // Intentamos hasta ~10s (5 lecturas potenciales si PERIOD_MS=2000)
    pollForRead(10000, 150);

    const bool healthy = driverPtr->healthy();
    const float t = tempPtr->getValue();
    const float h = humPtr->getValue();

    Serial.printf("[DHT22] healthy=%d, T=%.2f C, H=%.2f %%\n", (int)healthy, t, h);
    Serial.flush();

    TEST_ASSERT_TRUE_MESSAGE(healthy, "DHT22 no dio lectura valida (NaN). Revisa pull-up, cableado y periodo >=2s");
    TEST_ASSERT_FALSE(isnan(t));
    TEST_ASSERT_FALSE(isnan(h));

    // Rangos razonables (no hiper estrictos)
    TEST_ASSERT_TRUE_MESSAGE(t > -40.0f && t < 80.0f, "Temperatura fuera de rango esperado para DHT22");
    TEST_ASSERT_TRUE_MESSAGE(h >= 0.0f && h <= 100.0f, "Humedad fuera de rango [0,100]");
}

void test_dht_disable_stops_updates() {
    TEST_ASSERT_NOT_NULL(driverPtr);
    TEST_ASSERT_NOT_NULL(humPtr);
    TEST_ASSERT_NOT_NULL(tempPtr);

    // Conseguimos una lectura válida primero
    pollForRead(10000, 150);
    TEST_ASSERT_TRUE_MESSAGE(driverPtr->healthy(), "No hay lectura valida previa para probar disable");

    const unsigned long t0 = driverPtr->lastReadTime();
    TEST_ASSERT_TRUE_MESSAGE(t0 > 0, "lastReadTime() no se actualizo nunca");

    // Deshabilitamos ambos wrappers
    humPtr->setEnabled(false);
    tempPtr->setEnabled(false);

    // Esperamos más que el periodo y llamamos loop; no debería actualizar
    delay(PERIOD_MS + 300);
    humPtr->loop();
    tempPtr->loop();

    const unsigned long t1 = driverPtr->lastReadTime();

    Serial.printf("[DHT22] disable: lastReadTime before=%lu after=%lu\n", t0, t1);
    Serial.flush();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(t0, t1, "El driver se actualizo pese a estar deshabilitado");

    // Rehabilitamos
    humPtr->setEnabled(true);
    tempPtr->setEnabled(true);
}

void setup() {
    delay(1500);
    Serial.begin(115200);
    delay(200);

    Serial.println("[TEST] DHT22 (temp+hum) embedded tests");
    Serial.printf("[DHT22] pin=%u period=%lu ms\n", (unsigned)DHT_PIN, PERIOD_MS);
    Serial.flush();

    static Dht22Base driver(DHT_PIN);
    driverPtr = &driver;

    Dht22HumidityConfig hcfg;
    hcfg.readPeriodMs = PERIOD_MS;
    hcfg.friendlyName = "HumidityTest";
    hcfg.location = "mesa";

    Dht22TemperatureConfig tcfg;
    tcfg.readPeriodMs = PERIOD_MS;
    tcfg.friendlyName = "TempTest";
    tcfg.location = "mesa";

    static Dht22HumiditySensor hum(driver, hcfg);
    static Dht22TemperatureSensor temp(driver, tcfg);
    humPtr = &hum;
    tempPtr = &temp;

    UNITY_BEGIN();
    RUN_TEST(test_dht_begin_ok);
    RUN_TEST(test_dht_eventually_reads_valid_values);
    RUN_TEST(test_dht_disable_stops_updates);
    const int fails = UNITY_END();

    Serial.printf("[TEST] DHT22 finished. fails=%d\n", fails);
    Serial.flush();

    while (true) delay(1000);
}

void loop() {}
