#include <Connectivity/mqtt/handlers/setHandler.h>
#include <Connectivity/mqtt/MqttManager.h>
#include <Constants.h>
#include <ArduinoJson.h>

// Normaliza el campo "state" (bool/int/string) a un comando de actuador ("ON","OFF",...)
// Acepta:
//  - true/false  -> ON/OFF
//  - 1/0         -> ON/OFF
//  - "on"/"off"  -> ON/OFF
//  - "true"/"false" -> ON/OFF
//  - cualquier otro string se pasa tal cual (p.ej. "BEEP", "STOP", "FORWARD:60", etc.)
static bool stateToCommand(const JsonVariantConst &v, String &outCmd) {
    if (v.isNull()) return false;

    if (v.is<bool>()) {
        outCmd = v.as<bool>() ? "ON" : "OFF";
        return true;
    }

    if (v.is<int>() || v.is<long>() || v.is<float>() || v.is<double>()) {
        outCmd = (v.as<double>() != 0.0) ? "ON" : "OFF";
        return true;
    }

    if (v.is<const char*>()) {
        const char *s = v.as<const char*>();
        if (!s || !*s) return false;

        String cmd(s);
        cmd.trim();
        cmd.toUpperCase();

        if (cmd == "TRUE" || cmd == "1")  cmd = "ON";
        if (cmd == "FALSE" || cmd == "0") cmd = "OFF";

        outCmd = cmd;
        return true;
    }

    if (v.is<String>()) {
        String cmd = v.as<String>();
        cmd.trim();
        cmd.toUpperCase();

        if (cmd == "TRUE" || cmd == "1")  cmd = "ON";
        if (cmd == "FALSE" || cmd == "0") cmd = "OFF";

        outCmd = cmd;
        return true;
    }

    return false;
}

// Construye un comando desde {"command": "...", "speed": N}.
// Devuelve:
//  - "OPEN:100", "CLOSE:80" cuando hay speed
//  - "STOP" cuando no aplica speed
static bool commandToCommandString(const JsonObjectConst &obj, String &outCmd) {
    JsonVariantConst cv = obj["command"];
    if (cv.isNull()) return false;

    String cmd;

    if (cv.is<const char*>()) {
        const char *s = cv.as<const char*>();
        if (!s || !*s) return false;
        cmd = s;
    } else if (cv.is<String>()) {
        cmd = cv.as<String>();
    } else {
        return false;
    }

    cmd.trim();
    cmd.toUpperCase();
    if (cmd.length() == 0) return false;

    // Speed opcional (0-100)
    int speed = -1;
    JsonVariantConst sv = obj["speed"];
    if (!sv.isNull()) {
        if (sv.is<int>() || sv.is<long>()) {
            speed = sv.as<int>();
        } else if (sv.is<float>() || sv.is<double>()) {
            speed = (int)sv.as<double>();
        } else if (sv.is<const char*>()) {
            const char *ss = sv.as<const char*>();
            speed = ss ? String(ss).toInt() : -1;
        } else if (sv.is<String>()) {
            speed = sv.as<String>().toInt();
        }

        if (speed >= 0) speed = constrain(speed, 0, 100);
    }

    // STOP no necesita speed
    if (cmd == "STOP") {
        outCmd = "STOP";
        return true;
    }

    // Para OPEN/CLOSE/FORWARD/BACKWARD etc., añadimos speed si viene
    if (speed >= 0) {
        outCmd = cmd + ":" + String(speed);
    } else {
        outCmd = cmd;
    }

    return true;
}

// Handler principal invocado por el dispatcher
void SetHandler::handle(const HandlerContext &ctx) {

    // Validación de existencia
    if (!ctx.sensor && !ctx.actuator) {
        if (Constants::DEBUG)
            Serial.println("[SetHandler] ID no encontrado en sensores ni actuadores.");
        return;
    }

    // Redirección en función del type
    if (ctx.sensor) {
        handleSensorSet(ctx);
    } else if (ctx.actuator) {
        handleActuatorSet(ctx);
    }
}

// SET aplicado sobre un sensor (habilitado/deshabilitado)
void SetHandler::handleSensorSet(const HandlerContext &ctx) {
    JsonVariantConst en = (*ctx.json)["enable"];
    if (!en.is<bool>()) {
        if (Constants::DEBUG)
            Serial.println("[SetHandler] Payload SET para sensor inválido.");
        return;
    }

    const bool enable = en.as<bool>();
    ctx.sensor->setEnabled(enable);

    // Construcción de respuesta JSON
    JsonDocument response;
    response["enable"] = enable;
    response["requester"] = (*ctx.json)["requester"] | "unknown";

    publishResponse(ctx, response);
}

// SET aplicado sobre un actuador
void SetHandler::handleActuatorSet(const HandlerContext &ctx) {
    String cmd;
    bool parsed = false;

    // 1) Nuevo formato: {"command": "...", "speed": N}
    JsonVariantConst cv = (*ctx.json)["command"];
    if (!cv.isNull()) {
        JsonObjectConst obj = (*ctx.json).as<JsonObjectConst>();
        parsed = commandToCommandString(obj, cmd);

        if (!parsed && Constants::DEBUG) {
            Serial.println("[SetHandler] No se pudo interpretar 'command' (tipo no soportado).");
        }
    }

    // 2) Formato legacy: {"state": ...}
    if (!parsed) {
        JsonVariantConst st = (*ctx.json)["state"];
        if (st.isNull()) {
            if (Constants::DEBUG)
                Serial.println("[SetHandler] Payload SET para actuador inválido (falta state/command).");
            return;
        }

        parsed = stateToCommand(st, cmd);
        if (!parsed) {
            if (Constants::DEBUG)
                Serial.println("[SetHandler] No se pudo interpretar 'state' (tipo no soportado).");
            return;
        }
    }

    const bool ok = ctx.actuator->applyCommand(cmd.c_str());
    if (!ok && Constants::DEBUG) {
        Serial.printf("[SetHandler] applyCommand rechazado: %s\n", cmd.c_str());
    }

    // Construir respuesta JSON
    // Importante:
    // - "state" debe reflejar el ESTADO actual del actuador (estable si aplica),
    //   no el comando recibido (que puede incluir velocidad, p.ej. "OPEN:100").
    // - opcionalmente devolvemos "cmd" para depurar.
    JsonDocument response;
    response["state"] = ctx.actuator->stateString(); // p.ej.: OPEN / CLOSED / OPENING / CLOSING / STOP
    response["cmd"] = cmd;                           // opcional: OPEN:100 / CLOSE:80 / STOP / ON / OFF
    response["requester"] = (*ctx.json)["requester"] | "unknown";
    response["ok"] = ok;

    publishResponse(ctx, response);
}

// Publicar un tópico response según el formato esperado
void SetHandler::publishResponse(const HandlerContext &ctx, const JsonDocument &response) {
    String topic = "response/";
    topic += Constants::Mqtt::DEVICE_ID;
    topic += "/";
    topic += ctx.info.type;
    topic += "/";
    topic += ctx.info.id;

    String payload;
    serializeJson(response, payload);

    ctx.mqtt->publish(topic, payload);
}
