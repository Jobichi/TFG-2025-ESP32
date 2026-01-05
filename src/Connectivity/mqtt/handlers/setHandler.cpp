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

    // Por si ArduinoJson decide representarlo como String
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
    // Evita containsKey() (deprecated) y permite tipos no-string
    JsonVariantConst st = (*ctx.json)["state"];
    if (st.isNull()) {
        if (Constants::DEBUG)
            Serial.println("[SetHandler] Payload SET para actuador inválido (falta state).");
        return;
    }

    String cmd;
    if (!stateToCommand(st, cmd)) {
        if (Constants::DEBUG)
            Serial.println("[SetHandler] No se pudo interpretar 'state' (tipo no soportado).");
        return;
    }

    const bool ok = ctx.actuator->applyCommand(cmd.c_str());
    if (!ok && Constants::DEBUG) {
        Serial.printf("[SetHandler] applyCommand rechazado: %s\n", cmd.c_str());
    }

    // Construir respuesta JSON
    JsonDocument response;
    response["state"] = cmd; // devuelve el comando normalizado (ON/OFF/BEEP/...)
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
