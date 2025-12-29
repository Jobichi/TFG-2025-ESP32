#include <Connectivity/mqtt/publishers/AnnouncePublisher.h>

AnnouncePublisher::AnnouncePublisher(
    std::map<int, Sensor*> &sensors,
    std::map<int, Actuator*> &actuators,
    MqttManager &mqtt
)
    : sensors_(sensors), actuators_(actuators), mqtt_(&mqtt)
{}

// Publica announce para sensores y actuadores
void AnnouncePublisher::publishAll() {

    if (Constants::DEBUG)
        Serial.println("[AnnouncePublisher] Publicando announces...");

    for (auto &pair : sensors_) {
        publishSensor(pair.first, pair.second);
    }

    for (auto &pair : actuators_) {
        publishActuator(pair.first, pair.second);
    }
}

// ANNOUNCE para un sensor
void AnnouncePublisher::publishSensor(int id, Sensor *s) {

    String topic = "announce/";
    topic += Constants::Mqtt::DEVICE_ID;
    topic += "/sensor/";
    topic += id;

    StaticJsonDocument<128> doc;
    s->toAnnounceJson(doc);

    String payload;
    serializeJson(doc, payload);

    mqtt_->publish(topic, payload);

    if (Constants::DEBUG) {
        Serial.printf("[AnnouncePublisher] SENSOR ANNOUNCE => %s : %s\n",
                      topic.c_str(), payload.c_str());
    }
}

// ANNOUNCE para un actuador
void AnnouncePublisher::publishActuator(int id, Actuator *a) {

    String topic = "announce/";
    topic += Constants::Mqtt::DEVICE_ID;
    topic += "/actuator/";
    topic += id;

    StaticJsonDocument<128> doc;
    a->toAnnounceJson(doc);

    String payload;
    serializeJson(doc, payload);

    mqtt_->publish(topic, payload);

    if (Constants::DEBUG) {
        Serial.printf("[AnnouncePublisher] ACTUATOR ANNOUNCE => %s : %s\n",
                      topic.c_str(), payload.c_str());
    }
}
