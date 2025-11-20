#include <Connectivity/mqtt/publishers/AlertPublisher.h>

AlertPublisher::AlertPublisher(
    std::map<int, Sensor*> &sensors,
    std::map<int, Actuator*> &actuators,
    MqttManager &mqtt
)
    : sensors_(sensors), actuators_(actuators), mqtt_(&mqtt)
{}

// ----------------------------------------------
// ALERTA PARA SENSORES
// ----------------------------------------------
void AlertPublisher::publishSensorAlert(int id, const char* severity, const char* message, int code) {

    auto it = sensors_.find(id);
    if (it == sensors_.end()) {
        if (Constants::DEBUG)
            Serial.printf("[AlertPublisher] Sensor %d no encontrado.\n", id);
        return;
    }
    Sensor* s = it->second;

    String topic = "alert/";
    topic += Constants::Mqtt::DEVICE_ID;
    topic += "/sensor/";
    topic += id;

    StaticJsonDocument<256> doc;
    doc["severity"] = severity;
    doc["message"] = message;
    if (code != -1) doc["code"] = code;
    doc["name"] = s->name();
    doc["location"] = s->location();

    publishAlert(topic, doc);
}

// ----------------------------------------------
// ALERTA PARA ACTUADORES
// ----------------------------------------------
void AlertPublisher::publishActuatorAlert(int id, const char* severity, const char* message, int code) {

    auto it = actuators_.find(id);
    if (it == actuators_.end()) {
        if (Constants::DEBUG)
            Serial.printf("[AlertPublisher] Actuator %d no encontrado.\n", id);
        return;
    }
    Actuator* a = it->second;

    String topic = "alert/";
    topic += Constants::Mqtt::DEVICE_ID;
    topic += "/actuator/";
    topic += id;

    StaticJsonDocument<256> doc;
    doc["severity"] = severity;
    doc["message"] = message;
    if (code != -1) doc["code"] = code;
    doc["name"] = a->name();
    doc["location"] = a->location();

    publishAlert(topic, doc);
}

// ----------------------------------------------
// Publica el JSON en el tópico indicado
// ----------------------------------------------
void AlertPublisher::publishAlert(const String &topic, const JsonDocument &json) {

    String payload;
    serializeJson(json, payload);

    mqtt_->publish(topic, payload);

    if (Constants::DEBUG) {
        Serial.printf("[AlertPublisher] ALERT => %s : %s\n", topic.c_str(), payload.c_str());
    }
}
