#include <Arduino.h>
#include <unity.h>

#include <Devices/Actuators/Buzzer.h>

static constexpr uint8_t BUZZER_PIN = 13;
static Buzzer* buzzer = nullptr;

void test_sanity() {
    TEST_ASSERT_TRUE(true);
}

void test_buzzer_begin_ok() {
    TEST_ASSERT_NOT_NULL(buzzer);
    TEST_ASSERT_TRUE(buzzer->begin());
}

void test_buzzer_on_off_commands() {
    TEST_ASSERT_NOT_NULL(buzzer);

    // Asume comandos "ON"/"OFF" (como tu Relay).
    // Si tu Buzzer usa otros comandos, lo ajustamos.
    TEST_ASSERT_TRUE(buzzer->applyCommand("ON"));
    TEST_ASSERT_TRUE(buzzer->isActive());

    TEST_ASSERT_TRUE(buzzer->applyCommand("OFF"));
    TEST_ASSERT_FALSE(buzzer->isActive());
}

void setup() {
    delay(2500);
    Serial.begin(115200);
    delay(200);

    Serial.println("[TEST] test_buzzer: inicio");

    BuzzerConfig cfg;
    cfg.friendlyName = "BuzzerTest";
    cfg.location = "mesa";

    static Buzzer bz(BUZZER_PIN, cfg);
    buzzer = &bz;

    UNITY_BEGIN();
    RUN_TEST(test_sanity);
    RUN_TEST(test_buzzer_begin_ok);
    RUN_TEST(test_buzzer_on_off_commands);
    const int fails = UNITY_END();

    Serial.printf("[TEST] test_buzzer: fin fails=%d\n", fails);
    Serial.println("[TEST] Entrando en demo: ON 200ms / OFF 800ms ...");
    Serial.flush();
}

void loop() {
    static uint32_t last = 0;
    static bool on = false;

    const uint32_t now = millis();
    if (now - last >= (on ? 200UL : 800UL)) {
        last = now;
        on = !on;

        const bool ok = buzzer->applyCommand(on ? "ON" : "OFF");
        Serial.printf("[DEMO] cmd=%s ok=%d active=%d state=%s\n",
                      on ? "ON" : "OFF",
                      (int)ok,
                      (int)buzzer->isActive(),
                      buzzer->stateString().c_str());
        Serial.flush();
    }

    buzzer->loop();
}
