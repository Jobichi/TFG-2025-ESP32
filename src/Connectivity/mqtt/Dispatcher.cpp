#include <Connectivity/mqtt/Dispatcher.h>
#include <Constants.h>

Dispatcher::Dispatcher(
    std::map<int, Sensor *> &sensors,
    std::map<int, Actuator *> &actuators,
    MqttManager &mqtt
) : sensors_(sensors), actuators_(actuators), mqtt_(&mqtt) {}

//Registrar un handler para una acción concreta
void Dispatcher::registerHandler(const String &action, HandlerFunction handler) {
    handlers_[action] = handler;
}

// Función llamada desde MqttManager cuando llega un mensaje
void Dispatcher::route(const String &topic, const String &paylaod) {
    // Parseo del tópico
    TopicInfo info = TopicParser::parse(topic);

    if (!info.valid) {
        if (Constants::DEBUG)
            Serial.println("[Dispatcher] Tópico inválido.");
        return;
    }

    // Solo procesamos si el mensaje es para este dispositivo
    if (info.device != Constants::Mqtt::DEVICE_ID) {
        if (Constants::DEBUG)
            Serial.println("[Dispatcher] Ignorado: mensaje para otro dispositivo.");
        return;
    }

    handleMessage(info, paylaod);
}


void Dispatcher::handleMessage(const TopicInfo &info, const String &payload) {
    auto it = handlers_.find(info.action);
    if (it == handlers_.end()) {
        if (Constants::DEBUG)
            Serial.printf("[Dispatcher] No hay handler para acción: %s\n", info.action.c_str());
        return;
    }

    // Parseo del JSON
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, payload);

    if (err) {
        if (Constants::DEBUG)
            Serial.println("[Dispatcher] Error parseando JSON.");
        return;
    }

    // Creación del contexto para el handler:
    HandlerContext ctx;
    ctx.info = info;
    ctx.json = &doc;
    ctx.sensor = nullptr;
    ctx.actuator = nullptr;
    ctx.mqtt = mqtt_;

    // Determinar si el ID corresponse a sensor o actuador
    if(info.type == "sensor") {
        auto s = sensors_.find(info.id);
        if(s != sensors_.end())
            ctx.sensor = s->second;
    }
    else if (info.type == "actuator") {
        auto a = actuators_.find(info.id);
        if(a != actuators_.end())
            ctx.actuator = a->second;
    }

    // Llamada al handler
    it->second(ctx);
}