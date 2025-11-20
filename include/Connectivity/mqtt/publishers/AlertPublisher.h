#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <map>
#include <Devices/Sensors/Sensor.h>
#include <Devices/Actuators/Actuator.h>
#include <Connectivity/mqtt/MqttManager.h>
#include <Constants.h>

class AlertPublisher {
public:
    AlertPublisher(
        std::map<int, Sensor*> &sensors,
        std::map<int, Actuator*> &actuators,
        MqttManager &mqtt
    );

    // Publicación de alertas
    void publishSensorAlert(int id, const char* severity, const char* message, int code = -1);
    void publishActuatorAlert(int id, const char* severity, const char* message, int code = -1);

private:
    std::map<int, Sensor*> &sensors_;
    std::map<int, Actuator*> &actuators_;
    MqttManager* mqtt_;

    void publishAlert(const String &topic, const JsonDocument &json);
};
