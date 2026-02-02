#include <Arduino.h>
#include <unity.h>
#include <Wire.h>

#include <Devices/Sensors/Bmp280Sensor.h>

// Pines I2C típicos ESP32 DevKit
static constexpr uint8_t I2C_SDA = 21;
static constexpr uint8_t I2C_SCL = 22;

// Direcciones típicas BMP280
static constexpr uint8_t BMP_ADDR_1 = 0x76;
static constexpr uint8_t BMP_ADDR_2 = 0x77;

// Periodo corto para el test
static constexpr unsigned long READ_PERIOD_MS = 200;

static Bmp280Sensor* bmpPtr = nullptr;

static void waitAndPoll(Bmp280Sensor& s, unsigned long totalMs, unsigned long stepMs = 50) {
    const unsigned long start = millis();
    while (millis() - start < totalMs) {
        s.loop();
        delay(stepMs);
    }
}

void test_bmp280_begin_ok() {
    TEST_ASSERT_NOT_NULL(bmpPtr);

    const bool ok = bmpPtr->begin();
    Serial.printf("[BMP280] begin() => %d\n", (int)ok);
    Serial.flush();

    TEST_ASSERT_TRUE_MESSAGE(ok, "BMP280 no detectado (revisa SDA/SCL, VCC/GND y addr 0x76/0x77)");
}

void test_bmp280_units_and_defaults() {
    TEST_ASSERT_NOT_NULL(bmpPtr);

    TEST_ASSERT_TRUE(bmpPtr->isEnabled());
    TEST_ASSERT_EQUAL_STRING("hPa", bmpPtr->getUnits());
}

void test_bmp280_loop_updates_value_and_health() {
    TEST_ASSERT_NOT_NULL(bmpPtr);

    // Espera suficiente para que se ejecute al menos 1 lectura (varias ventanas por si acaso)
    waitAndPoll(*bmpPtr, READ_PERIOD_MS * 8, 50);

    const float p = bmpPtr->getValue();          // presión en hPa (tu valor principal)
    const float t = bmpPtr->lastTemperatureC();  // extra debug (opcional)

    Serial.printf("[BMP280] p=%.2f hPa, t=%.2f C, healthy=%d\n", p, t, (int)bmpPtr->isHealthy());
    Serial.flush();

    TEST_ASSERT_FALSE_MESSAGE(isnan(p), "Presion NaN: no se actualizo la lectura o fallo el sensor");
    TEST_ASSERT_TRUE_MESSAGE(p > 0.0f, "Presion <= 0: lectura inesperada");
    TEST_ASSERT_TRUE_MESSAGE(bmpPtr->isHealthy(), "Sensor no healthy despues de leer (posible NaN o fallo I2C)");
}

void test_bmp280_disable_stops_updates() {
    TEST_ASSERT_NOT_NULL(bmpPtr);

    // Aseguramos una lectura inicial
    waitAndPoll(*bmpPtr, READ_PERIOD_MS * 6, 50);
    const float before = bmpPtr->getValue();
    TEST_ASSERT_FALSE(isnan(before));

    // Desactivamos y esperamos: no debería cambiar el valor (o al menos no debería hacer lecturas)
    bmpPtr->setEnabled(false);
    waitAndPoll(*bmpPtr, READ_PERIOD_MS * 6, 50);
    const float after = bmpPtr->getValue();

    Serial.printf("[BMP280] disabled: before=%.2f after=%.2f\n", before, after);
    Serial.flush();

    // Como no hay nuevas lecturas, debería mantenerse igual (tolerancia por si tu implementación cambia)
    TEST_ASSERT_FLOAT_WITHIN(0.01f, before, after);

    // Rehabilitamos para no dejarlo apagado si reusas el objeto
    bmpPtr->setEnabled(true);
}

void setup() {
    delay(1500);
    Serial.begin(115200);
    delay(200);

    Serial.println("[TEST] BMP280 embedded tests");
    Serial.flush();

    Wire.begin(I2C_SDA, I2C_SCL);
    delay(50);

    Bmp280Config cfg76;
    cfg76.i2cAddress = BMP_ADDR_1;
    cfg76.readPeriodMs = READ_PERIOD_MS;
    cfg76.friendlyName = "Bmp280Test";
    cfg76.location = "mesa";

    Bmp280Config cfg77 = cfg76;
    cfg77.i2cAddress = BMP_ADDR_2;

    static Bmp280Sensor bmp76(cfg76);
    static Bmp280Sensor bmp77(cfg77);

    // Autodetección por begin()
    if (bmp76.begin()) {
        bmpPtr = &bmp76;
        Serial.println("[BMP280] Using addr 0x76");
    } else if (bmp77.begin()) {
        bmpPtr = &bmp77;
        Serial.println("[BMP280] Using addr 0x77");
    } else {
        bmpPtr = &bmp76; // para que los asserts den mensaje claro
        Serial.println("[BMP280] Not detected at 0x76 nor 0x77");
    }
    Serial.flush();

    UNITY_BEGIN();

    // OJO: como ya hicimos begin() en autodetección, este test solo comprueba que hay begin OK “en general”.
    // Si prefieres que test_bmp280_begin_ok llame begin(), quita el begin() de arriba y fija una dirección.
    RUN_TEST(test_bmp280_begin_ok);
    RUN_TEST(test_bmp280_units_and_defaults);
    RUN_TEST(test_bmp280_loop_updates_value_and_health);
    RUN_TEST(test_bmp280_disable_stops_updates);

    const int fails = UNITY_END();
    Serial.printf("[TEST] BMP280 finished. fails=%d\n", fails);
    Serial.flush();

    while (true) delay(1000);
}

void loop() {}
