#pragma once
#include <Arduino.h>
#include <map>
#include <functional>
#include <ArduinoJson.h>
#include <Connectivity/mqtt/TopicParser.h>
#include <Connectivity/mqtt/MqttManager.h>
#include <Devices/Actuators/Actuator.h>
#include <Devices/Sensors/Sensor.h>

// Contexto enviado a cada handler:
struct HandlerContext {
    TopicInfo info;         // Tópico parseado
    JsonDocument *json;     // Payload JSON parseado
    Sensor *sensor;         // no nulo si aplica a sensor
    Actuator *actuator;     // no nulo si aplica a actuador
    MqttManager *mqtt;      // Para publicar responses
};

// Firma base para handlers
using HandlerFunction = std::function<void(const HandlerContext &)>;

class Dispatcher {
    public:
        Dispatcher(
            std::map<int, Sensor*> &sensors,
            std::map<int, Actuator*> &actuators,
            MqttManager &mqtt
        );
        
        // Registrar handler para una acción (set, get, update)
        void registerHandler(const String &action, HandlerFunction handler);
        
        // Puntero de entrada desde MQTTManager
        void route(const String &topic, const String &payload);

    private:
        std::map<int, Sensor *> &sensors_;
        std::map<int, Actuator *> &actuators_;
        std::map<String, HandlerFunction> handlers_;
        MqttManager* mqtt_;

        void handleMessage(const TopicInfo &info, const String &payload);
};