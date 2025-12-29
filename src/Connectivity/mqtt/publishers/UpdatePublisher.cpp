#include <Connectivity/mqtt/publishers/UpdatePublisher.h>

UpdatePublisher::UpdatePublisher(
    std::map<int, Sensor*> &sensors,
    std::map<int, Actuator*> &actuators,
    MqttManager &mqtt
)
    : sensors_(sensors), actuators_(actuators), mqtt_(&mqtt), enabled_(false)
{}

// Publica update para todos los sensores y actuadores
void UpdatePublisher::publishAll() {

    if (!enabled_) {
        if (Constants::DEBUG)
            Serial.println("[UpdatePublisher] Saltando update: announce pendiente");
        return;
    }

    if (Constants::DEBUG)
        Serial.println("[UpdatePublisher] Publicando updates...");

    for (auto &pair : sensors_) {
        publishSensor(pair.first, pair.second);
    }

    for (auto &pair : actuators_) {
        publishActuator(pair.first, pair.second);
    }
}

// UPDATE de sensor
void UpdatePublisher::publishSensor(int id, Sensor *s) {

    // Respeta tu arquitectura: sensores deshabilitados NO deben publicar update
    if (!s->isEnabled())
        return;

    String topic = "update/";
    topic += Constants::Mqtt::DEVICE_ID;
    topic += "/sensor/";
    topic += id;

    StaticJsonDocument<256> doc;
    s->toUpdateJson(doc);

    String payload;
    serializeJson(doc, payload);

    mqtt_->publish(topic, payload);

    if (Constants::DEBUG) {
        Serial.printf("[UpdatePublisher] SENSOR UPDATE => %s : %s\n",
                      topic.c_str(), payload.c_str());
    }
}

// UPDATE de actuador
void UpdatePublisher::publishActuator(int id, Actuator *a) {

    String topic = "update/";
    topic += Constants::Mqtt::DEVICE_ID;
    topic += "/actuator/";
    topic += id;

    StaticJsonDocument<128> doc;
    a->toUpdateJson(doc);

    String payload;
    serializeJson(doc, payload);

    mqtt_->publish(topic, payload);

    if (Constants::DEBUG) {
        Serial.printf("[UpdatePublisher] ACTUATOR UPDATE => %s : %s\n",
                      topic.c_str(), payload.c_str());
    }
}

// Permite activar/desactivar la publicación (p.ej. esperar a announce)
void UpdatePublisher::setEnabled(bool enabled) {
    enabled_ = enabled;
}
