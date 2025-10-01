#include "Connectivity/MqttManager.h"
#include "Logic/CommandHandler.h"
#include "Logic/SensorManager.h"

static WiFiClient espClient;
PubSubClient mqttClient(espClient);

static unsigned long lastReconnectAttempt = 0;

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String msg;
    for (unsigned int i = 0; i < length; i++) {
        msg += (char)payload[i];
    }

    #if DEBUG
        Serial.printf("[MQTT] Mensaje recibido en [%s]: %s\n", topic, msg.c_str());
    #endif

    String topicStr = String(topic);
    
    // 1. Manejar PONG
    String pongTopic = String(DEVICE_ID) + "/pong";
    if (topicStr == pongTopic) {
        #if DEBUG
            Serial.printf("[MQTT] PONG recibido → conectividad confirmada\n");
        #endif
        return;  // IMPORTANTE: salir después de manejar
    }
    
    // 2. Manejar COMANDOS de actuadores
    String cmdTopic = "cmd/" + String(DEVICE_ID) + "/actuators";
    if (topicStr == cmdTopic) {
        handleMqttCommand(topicStr, msg);
        return;  // IMPORTANTE: salir después de manejar
    }
    
    // 3. Manejar PETICIONES GET de sensores  
    if (topicStr.startsWith("get/" + String(DEVICE_ID) + "/")) {
        handleSensorRequest(topicStr, msg);
        return;  // IMPORTANTE: salir después de manejar
    }
    
    #if DEBUG
        Serial.printf("[MQTT] Tópico no manejado: %s\n", topic);
    #endif
}

static void reconnectMqtt() {
    if (mqttClient.connected()) return;

    #if DEBUG
        Serial.print("[MQTT] Intentando conexión... ");
    #endif

    if (mqttClient.connect(MQTTT_CLIENT_ID, MQTT_USER, MQTT_PASS)) {
        #if DEBUG
            Serial.println("¡Conectado!");
        #endif

        // Suscribirse al PONG del propio dispositivo
        String pongTopic = String(DEVICE_ID) + "/pong";
        mqttSubscribe(pongTopic.c_str());

        // Enviar un único PING para comprobar conectividad
        String pingTopic = String(DEVICE_ID) + "/ping";
        mqttPublish(pingTopic.c_str(), "ping");

        // Subscripción al canal de comandos para este ESP32:
        String cmdTopic = "cmd/" + String(DEVICE_ID) + "/actuators";
        mqttSubscribe(cmdTopic.c_str());

        // Subscripción al canal de peticiones GET:
        String getTopic = String("get/") + DEVICE_ID + "/#";
        mqttSubscribe(getTopic.c_str());

    } else {
        #if DEBUG
            Serial.printf("Fallo rc=%d, reintento en 5s\n", mqttClient.state());
        #endif
    }
}

void setupMqtt() {
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);
    reconnectMqtt();
}

void handleMqtt() {
    if (!mqttClient.connected()) {
        unsigned long now = millis();
        if (now - lastReconnectAttempt > 5000) {
            lastReconnectAttempt = now;
            reconnectMqtt();
        }
    } else {
        mqttClient.loop();
    }
}

bool mqttPublish(const char* topic, const char* payload, bool retained) {
    if (!mqttClient.connected()) {
        #if DEBUG
            Serial.printf("[MQTT] ERROR: No conectado para publicar en [%s]\n", topic);
        #endif
        return false;
    }
    
    bool success = mqttClient.publish(topic, payload, retained);
    
    #if DEBUG
        if (success) {
            Serial.printf("[MQTT] Publicado en [%s]: %s\n", topic, payload);
        } else {
            Serial.printf("[MQTT] ERROR al publicar en [%s]\n", topic);
        }
    #endif
    
    return success;
}

bool mqttSubscribe(const char* topic) {
    if (!mqttClient.connected()) return false;
    #if DEBUG
        Serial.printf("[MQTT] Suscrito a [%s]\n", topic);
    #endif
    return mqttClient.subscribe(topic);
}

bool isMqttConnected(){
    return mqttClient.connected();
}