#include <Arduino.h>
#include <unity.h>
#include "networks/WiFiManager.hpp"

const char* TEST_SSID = "R-73";
const char* TEST_PASSWD = "20924273";

WiFiManager wifi(TEST_SSID, TEST_PASSWD);

// Test 1: Estado inicial de desconectado.
void test_wifi_initial_state(){
    TEST_ASSERT_FALSE(wifi.isConnected());
}

// Test 2: Intenta conectar y debería de conectarse en <= 10s
void test_wifi_connect(){
    wifi.connect();

    unsigned long start = millis();
    while(!wifi.isConnected() && (millis() - start) < 10000){
        wifi.checkConnection();
        delay(500);
    }

    TEST_ASSERT_TRUE(wifi.isConnected());
}

// Test 3: Forzar desconexión
void test_wifi_disconnect(){
    wifi.disconnect();
    delay(1000);
    TEST_ASSERT_FALSE(wifi.isConnected());
}

void setup(){
    Serial.begin(115200);
    UNITY_BEGIN();

    RUN_TEST(test_wifi_initial_state);
    RUN_TEST(test_wifi_connect);
    RUN_TEST(test_wifi_disconnect);

    UNITY_END();
}

void loop() {}