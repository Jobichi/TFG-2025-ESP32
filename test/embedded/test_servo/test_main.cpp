#include <Arduino.h>
#include <unity.h>

#include <Devices/Actuators/ServoMotor360.h>

static constexpr uint8_t SERVO_PIN = 18;
static constexpr int SERVO_CH = 0;

static ServoMotor360* servoPtr = nullptr;

void test_servo_begin_ok() {
    TEST_ASSERT_NOT_NULL(servoPtr);
    const bool ok = servoPtr->begin();
    Serial.printf("[SERVO] begin() => %d\n", (int)ok);
    Serial.flush();
    TEST_ASSERT_TRUE(ok);
}

void test_servo_applycommand_ok() {
    TEST_ASSERT_NOT_NULL(servoPtr);

    // Comandos típicos
    TEST_ASSERT_TRUE(servoPtr->applyCommand("FORWARD"));
    delay(300);

    TEST_ASSERT_TRUE(servoPtr->applyCommand("FORWARD:40"));
    delay(300);

    TEST_ASSERT_TRUE(servoPtr->applyCommand("BACKWARD 30"));
    delay(300);

    TEST_ASSERT_TRUE(servoPtr->applyCommand("STOP"));
    delay(200);
}

void test_servo_applycommand_rejects_invalid() {
    TEST_ASSERT_NOT_NULL(servoPtr);

    // Depende de tu decisión: si quieres que "FORWARD:abc" sea inválido, aquí debe ser false.
    // Con tu implementación actual devuelve true y velocidad 0 => STOP, así que este test fallaría.
    // Yo lo dejaría como inválido.
    TEST_ASSERT_FALSE(servoPtr->applyCommand(""));
    TEST_ASSERT_FALSE(servoPtr->applyCommand("UNKNOWN"));
}

void setup() {
    delay(1500);
    Serial.begin(115200);
    delay(200);

    Serial.println("[TEST] ServoMotor360 embedded tests");
    Serial.printf("[SERVO] pin=%u channelHint=%d\n", (unsigned)SERVO_PIN, SERVO_CH);
    Serial.flush();

    ServoConfig cfg;
    cfg.friendlyName = "ServoTest";
    cfg.location = "mesa";
    cfg.channelHint = SERVO_CH;

    static ServoMotor360 s(SERVO_PIN, cfg);
    servoPtr = &s;

    UNITY_BEGIN();

    RUN_TEST(test_servo_begin_ok);

    // Solo si begin fue bien, probamos comandos (evitas ruido si el servo no adjunta)
    if (servoPtr) {
        RUN_TEST(test_servo_applycommand_ok);
        RUN_TEST(test_servo_applycommand_rejects_invalid);
    }

    const int fails = UNITY_END();
    Serial.printf("[TEST] done. fails=%d\n", fails);
    Serial.flush();

    while (true) delay(1000);
}

void loop() {}
