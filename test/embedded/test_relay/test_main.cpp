#include <Arduino.h>
#include <unity.h>

#include <Devices/Actuators/Relay.h>

static constexpr uint8_t RELAY_PIN = 23; // ajusta a tu pin real
static Relay* relayPtr = nullptr;

void test_relay_begin_ok() {
    TEST_ASSERT_NOT_NULL(relayPtr);
    TEST_ASSERT_TRUE(relayPtr->begin());
}

void test_relay_initial_state_off() {
    TEST_ASSERT_NOT_NULL(relayPtr);
    TEST_ASSERT_FALSE(relayPtr->isActive());
    TEST_ASSERT_EQUAL_STRING("OFF", relayPtr->stateString().c_str());
}

void test_relay_on_off_commands() {
    TEST_ASSERT_NOT_NULL(relayPtr);

    TEST_ASSERT_TRUE(relayPtr->applyCommand("ON"));
    TEST_ASSERT_TRUE(relayPtr->isActive());
    TEST_ASSERT_EQUAL_STRING("ON", relayPtr->stateString().c_str());

    // Mantener el relé activo más tiempo para verificar físicamente el estado
    delay(5000);

    TEST_ASSERT_TRUE(relayPtr->applyCommand("OFF"));
    TEST_ASSERT_FALSE(relayPtr->isActive());
    TEST_ASSERT_EQUAL_STRING("OFF", relayPtr->stateString().c_str());
}

void test_relay_rejects_unknown_command() {
    TEST_ASSERT_NOT_NULL(relayPtr);
    TEST_ASSERT_FALSE(relayPtr->applyCommand("INVALID"));
}

void setup() {
    delay(2000);
    Serial.begin(115200);
    delay(200);

    RelayConfig cfg;
    cfg.activeLow = true;              // tu módulo es active-low
    cfg.friendlyName = "RelayTest";
    cfg.location = "mesa";

    static Relay r(RELAY_PIN, cfg);
    relayPtr = &r;

    UNITY_BEGIN();
    RUN_TEST(test_relay_begin_ok);
    RUN_TEST(test_relay_initial_state_off);
    RUN_TEST(test_relay_on_off_commands);
    RUN_TEST(test_relay_rejects_unknown_command);
    const int fails = UNITY_END();

    Serial.printf("[TEST] Relay finished. fails=%d\n", fails);
    Serial.flush();
    while (true) delay(1000);
}

void loop() {}
