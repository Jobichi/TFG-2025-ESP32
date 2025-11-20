#include <Connectivity/mqtt/MqttManager.h>

MqttManager::MqttManager(WifiManager &wifi) :
    wifiManager_(wifi), mqttClient_(wifiClient_){}

void MqttManager::begin(){
    mqttClient_.setServer(
        Constants::Mqtt::MQTT_HOST,
        Constants::Mqtt::MQTT_PORT
    );

    mqttClient_.setCallback(
        [this](char *topic, byte *payload, unsigned int length)
        {
            this->internalCallback(topic, payload, length);
        }
    );

    if (Constants::DEBUG) {
        Serial.println("[MQTT-Manager] Inicializando módulo MQTT...");
    }
}

bool MqttManager::isConnected(){
    return mqttClient_.connected();
}

void MqttManager::loop(){
    if (!wifiManager_.isConnected()) return;

    if(!isConnected()) {
        unsigned long now = millis();
        if (now - lastReconnectAttempt_ >= reconnectInterval_) {
            lastReconnectAttempt_ = now;
            connectToBroker();
        }
    }

    mqttClient_.loop();
}

void MqttManager::connectToBroker(){
    if (Constants::DEBUG) {
        Serial.println("[MQTT-Manager] Intentando conectar al broker...");
    }

    if(mqttClient_.connect(
        Constants::Mqtt::DEVICE_ID,
        Constants::Mqtt::MQTT_USER,
        Constants::Mqtt::MQTT_PASS
    )) {
        if (Constants::DEBUG) {
            Serial.println("[MQTT-Manager] Conexión MQTT exitosa.");
        }

        for (const auto &entry : subHandlers_) {
            mqttClient_.subscribe(entry.first.c_str());
            if (Constants::DEBUG) {
                Serial.printf("[MQTT-Manager] Subscripción a: %s\n", entry.first.c_str());
            }
        }
    } else if (Constants::DEBUG){
        Serial.printf("[MQTT-Manager] Error conexión. Código: %d\n", mqttClient_.state());
    }
}

void MqttManager::addSubHandler(const String &topic, MqttCallback callback){
    subHandlers_[topic] = callback;

    if (isConnected()) {
        mqttClient_.subscribe(topic.c_str());
    }
}

void MqttManager::addPubHandler(const String &topic, MqttCallback callback){
    pubHandlers_[topic] = callback;
}

void MqttManager::publish(const String &topic, const String &payload, bool retained){
    if(!isConnected())
        return;

    mqttClient_.publish(topic.c_str(), payload.c_str(), retained);

    auto it = pubHandlers_.find(topic);
    if (it != pubHandlers_.end()){
        it->second(topic, payload);
    }
}

void MqttManager::internalCallback(char *topic, byte *payload, unsigned int length) {
    String topicStr = String(topic);
    String payloadStr;
    payloadStr.reserve(length);

    for (unsigned int i = 0; i < length; i++) {
        payloadStr += (char)payload[i];
    }

    if (Constants::DEBUG) {
        Serial.printf("[MQTT-Manager] Recibido: %s => %s\n", topicStr.c_str(), payloadStr.c_str());
    }

    bool handlerFound = false;

    for (auto &entry : subHandlers_) {

        const String &pattern = entry.first;

        // Coincidencia exacta
        if (pattern == topicStr) {
            entry.second(topicStr, payloadStr);
            handlerFound = true;
            break;
        }

        // Coincidencia wildcard get/# → prefijo
        if (pattern.endsWith("/#")) {
            String prefix = pattern.substring(0, pattern.length() - 2); // "get/"
            if (topicStr.startsWith(prefix)) {
                entry.second(topicStr, payloadStr);
                handlerFound = true;
                break;
            }
        }
    }

    if (!handlerFound && Constants::DEBUG) {
        Serial.println("[MQTT-Manager] No hay handler para este tópico.");
    }
}