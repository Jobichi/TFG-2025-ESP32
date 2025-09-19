#include "Connectivity/MqttManager.h"
#include "Connectivity/WifiManager.h"

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

    // Si recibimos un ping, respondemos con un pong
    String pingTopic = String(DEVICE_ID) + "/ping";
    if (String(topic) == pingTopic) {
        String pongTopic = String(DEVICE_ID) + "/pong";
        mqttPublish(pongTopic.c_str(), "pong");
        #if DEBUG
            Serial.printf("[MQTT] Respondido con PONG en [%s]\n", pongTopic.c_str());
        #endif
    }
}

static void reconnectMqtt() {
    if (mqttClient.connected()) return;

    #if DEBUG
        Serial.print("[MQTT] Intentando conexión...");
    #endif

    if (mqttClient.connect(MQTTT_CLIENT_ID, MQTT_USER, MQTT_PASS)) {
        #if DEBUG
            Serial.println("[MQTT] ¡Conectado!");
        #endif

        // Subscripción de inicialización para testeo automático.
        String pingTopic = String(DEVICE_ID) + "/ping";
        mqttSubscribe(pingTopic.c_str());   
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
    if (!mqttClient.connected()){
        unsigned long now = millis();

        if (now - lastReconnectAttempt > 5000) {
            lastReconnectAttempt = now;
            reconnectMqtt();
        }
    } else {
        mqttClient.loop();
    }
}

bool mqttPublish (const char* topic, const char* payload, bool retained) {
    if (!mqttClient.connected()) return false;
    #if DEBUG
        Serial.printf("[MQTT] Publicando en [%s]: %s\n", topic, payload);
    #endif
    return mqttClient.publish(topic, payload, retained);
}

bool mqttSubscribe(const char* topic) {
    if (!mqttClient.connected()) return false;
    #if DEBUG
        Serial.printf("[MQTT] Suscrito a [%s]\n", topic);
    #endif
    return mqttClient.subscribe(topic);
}