#include <Connectivity/mqtt/publishers/UpdatePublisher.h>
#include <Constants.h>
#include <ArduinoJson.h>

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

    // Sensores deshabilitados NO deben publicar update
    if (!s || !s->isEnabled())
        return;

    String topic = "update/";
    topic += Constants::Mqtt::DEVICE_ID;
    topic += "/sensor/";
    topic += id;

    JsonDocument doc;
    s->toUpdateJson(doc);

    String payload;
    payload.reserve(256);
    serializeJson(doc, payload);

    mqtt_->publish(topic, payload);

    if (Constants::DEBUG) {
        Serial.printf("[UpdatePublisher] SENSOR UPDATE => %s : %s\n",
                      topic.c_str(), payload.c_str());
    }
}

// UPDATE de actuador
void UpdatePublisher::publishActuator(int id, Actuator *a) {

    if (!a)
        return;

    String topic = "update/";
    topic += Constants::Mqtt::DEVICE_ID;
    topic += "/actuator/";
    topic += id;

    JsonDocument doc;
    a->toUpdateJson(doc);

    String payload;
    payload.reserve(128);
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
