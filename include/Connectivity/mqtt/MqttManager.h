#pragma once

#include <map>
#include <functional>
#include <PubSubClient.h>
#include <Connectivity/Connectivity.h>
#include <Connectivity/wifi/WifiManager.h>
#include <Constants.h>


class MqttManager : public Connectivity{
    public:
        explicit MqttManager(WifiManager &wifi);

        void begin() override;
        void loop() override;
        bool isConnected() override;

        using MqttCallback = std::function<void(const String &, const String &)>;

        void addSubHandler(const String &topic, MqttCallback);
        void addPubHandler(const String &topic, MqttCallback);

        void publish(const String &topic, const String &payload, bool retained = false);

    private:
        WifiManager &wifiManager_;
        WiFiClient wifiClient_;
        PubSubClient mqttClient_;

        std::map<String, MqttCallback> subHandlers_;
        std::map<String, MqttCallback> pubHandlers_;

        unsigned long lastReconnectAttempt_ = 0;
        const unsigned long reconnectInterval_ = 5000;

        void connectToBroker();
        void internalCallback(char *topic, byte *payload, unsigned int length);
};
