#pragma once
#include <map>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Constants.h>
#include <Devices/Sensors/Sensor.h>
#include <Devices/Actuators/Actuator.h>
#include <Connectivity/mqtt/MqttManager.h>

class UpdatePublisher {
public:
    UpdatePublisher(
        std::map<int, Sensor*> &sensors,
        std::map<int, Actuator*> &actuators,
        MqttManager &mqtt
    );

    // Publica UPDATE de todos los sensores y actuadores
    void publishAll();
    // Habilita/deshabilita publicación (p.ej. esperar a announce inicial)
    void setEnabled(bool enabled = true);

private:
    void publishSensor(int id, Sensor *s);
    void publishActuator(int id, Actuator *a);

    std::map<int, Sensor*> &sensors_;
    std::map<int, Actuator*> &actuators_;
    MqttManager *mqtt_;
    bool enabled_ = false;
};
