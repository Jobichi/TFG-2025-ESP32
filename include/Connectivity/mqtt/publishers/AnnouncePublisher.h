#pragma once
#include <map>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Constants.h>
#include <Devices/Sensors/Sensor.h>
#include <Devices/Actuators/Actuator.h>
#include <Connectivity/mqtt/MqttManager.h>

class AnnouncePublisher {
public:
    AnnouncePublisher(
        std::map<int, Sensor*> &sensors,
        std::map<int, Actuator*> &actuators,
        MqttManager &mqtt
    );

    // Publica announce para todos los dispositivos
    void publishAll();

private:
    void publishSensor(int id, Sensor *s);
    void publishActuator(int id, Actuator *a);

    std::map<int, Sensor*> &sensors_;
    std::map<int, Actuator*> &actuators_;
    MqttManager *mqtt_;
};
