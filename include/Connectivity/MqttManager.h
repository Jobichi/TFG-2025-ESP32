#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "configCredentials.h"
#include "Sensors/SensorBase.h"
#include "Actuators/ActuatorBase.h"
#include "Connectivity/mqtt/setManager.h"
#include "Connectivity/mqtt/getManager.h"
#include "Connectivity/mqtt/announceManager.h"

// Alias para punteros a funciones manejadoras de MQTT
typedef void(*MqttHandler)(const String& topic, const String& payload);

// Inicialización y gestión MQTT
void setupMqtt();
void handleMqtt();

// Publicar mensajes
bool mqttPublish(const char* topic, const char* payload, bool retained = false);

// Suscripción a tópicos
bool mqttSubscribe(const char* topic);

// Comprobación de conectividad
bool isMqttConnected();