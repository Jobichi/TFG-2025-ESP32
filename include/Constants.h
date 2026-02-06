#pragma once
#include <cstdint>

namespace Constants {
    // Wifi Glovals
    namespace Wifi {
        // Wifi Credentials
        constexpr const char *WIFI_SSID = "R-73";
        constexpr const char *WIFI_PASS = "20924273";

        // Wifi Parammeters
        constexpr uint8_t WIFI_IP[4] = {10, 12, 45, 20};
        constexpr uint8_t WIFI_GATEWAY[4] = {192, 168, 1, 1};
        constexpr uint8_t WIFI_SUBNET[4] = {255, 255, 255, 0};
    }

    namespace Mqtt {
        // Mqtt Credentials
        constexpr const char *MQTT_USER = "admin";
        constexpr const char *MQTT_PASS = "admin1234";
        constexpr const char *DEVICE_ID = "esp32_cocina";

        // Mqtt Parammeters
        constexpr const char *MQTT_HOST = "10.12.45.1";
        constexpr uint16_t MQTT_PORT = 1883;
    }

    // Debug constant var
    constexpr const bool DEBUG = true;
}