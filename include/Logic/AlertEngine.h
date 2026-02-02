// include/Logic/AlertEngine.h  (o donde tengas la clase)
#pragma once

#include <Arduino.h>
#include <vector>
#include <map>

#include <Logic/DeviceRegistry.h>
#include <Connectivity/mqtt/publishers/AlertPublisher.h>

class AlertEngine {
public:
    enum class Type : uint8_t {
        Hysteresis,      // recomendado (stateful)
        BooleanChange    // alterna mensajes on/off según estado (stateful)
    };

    struct Rule {
        Type type;
        int sensorId;

        // Histeresis
        float low = 0.0f;
        float high = 1.0f;

        // Estado interno
        bool initialized = false;
        bool state = false; // 0/1 latcheado

        // Mensajes para transiciones (Hysteresis)
        const char* severityRise = "warning";
        const char* msgRise = "RISING";
        int codeRise = -1;

        const char* severityFall = "info";
        const char* msgFall = "FALLING";
        int codeFall = -1;
    };

    AlertEngine(DeviceRegistry& reg, AlertPublisher& publisher)
        : reg_(reg), publisher_(publisher) {}

    // Histeresis: envía rising cuando cruza high, falling cuando cruza low.
    // - low < high (si vienen invertidos, los reordenamos).
    // - No publica en la inicialización: solo en cambios posteriores.
    void addHysteresis(
        int sensorId,
        float lowThreshold,
        float highThreshold,
        const char* severityRise, const char* msgRise, int codeRise,
        const char* severityFall, const char* msgFall, int codeFall
    ) {
        Rule r;
        r.type = Type::Hysteresis;
        r.sensorId = sensorId;

        if (lowThreshold <= highThreshold) {
            r.low = lowThreshold;
            r.high = highThreshold;
        } else {
            r.low = highThreshold;
            r.high = lowThreshold;
        }

        r.severityRise = severityRise;
        r.msgRise = msgRise;
        r.codeRise = codeRise;

        r.severityFall = severityFall;
        r.msgFall = msgFall;
        r.codeFall = codeFall;

        rules_.push_back(r);
    }

    // BooleanChange: interpreta value >= threshold como true y envía ON/OFF solo en cambio.
    // Es básicamente histeresis con low=high, pero más cómodo para binarios.
    void addBooleanChange(
        int sensorId,
        float threshold,
        const char* severityOn, const char* msgOn, int codeOn,
        const char* severityOff, const char* msgOff, int codeOff
    ) {
        addHysteresis(sensorId, threshold, threshold, severityOn, msgOn, codeOn, severityOff, msgOff, codeOff);
        rules_.back().type = Type::BooleanChange;
    }

    void loop() {
        for (size_t i = 0; i < rules_.size(); ++i) {
            Rule& rule = rules_[i];

            std::map<int, Sensor*>& smap = reg_.sensors();
            std::map<int, Sensor*>::iterator it = smap.find(rule.sensorId);
            if (it == smap.end() || it->second == nullptr) continue;

            Sensor* s = it->second;
            if (!s->isEnabled()) continue;

            const float v = s->getValue();

            if (!rule.initialized) {
                // Inicialización del estado (sin publicar)
                // - Para histeresis: arrancamos en true si ya está por encima de high.
                // - Si está entre low/high, queda false (conservador).
                rule.state = (v >= rule.high);
                rule.initialized = true;
                continue;
            }

            bool newState = rule.state;

            if (rule.type == Type::Hysteresis) {
                // Estado con banda
                if (!rule.state && v >= rule.high) newState = true;
                else if (rule.state && v <= rule.low) newState = false;
            } else {
                // BooleanChange: umbral “duro”
                // (low==high en addBooleanChange)
                newState = (v >= rule.high);
            }

            if (newState != rule.state) {
                // Transición => una única alerta por cambio
                if (newState) {
                    publisher_.publishSensorAlert(rule.sensorId, rule.severityRise, rule.msgRise, rule.codeRise);
                } else {
                    publisher_.publishSensorAlert(rule.sensorId, rule.severityFall, rule.msgFall, rule.codeFall);
                }
                rule.state = newState;
            }
        }
    }

private:
    DeviceRegistry& reg_;
    AlertPublisher& publisher_;
    std::vector<Rule> rules_;
};
