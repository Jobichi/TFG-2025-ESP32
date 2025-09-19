#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "configCredentials.h"

// Inicialización y gestión MQTT
void setupMqtt();
void handleMqtt();

// Publicar mensajes
bool mqttPublish(const char* topic, const char* payload, bool retained = false);

// Suscripción a tópicos
bool mqttSubscribe(const char* topic);

// Callback que usa PubSubClient (firma fija)
void mqttCallback(char* topic, byte* payload, unsigned int length);
