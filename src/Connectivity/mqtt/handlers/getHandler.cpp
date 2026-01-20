#include <Connectivity/mqtt/handlers/getHandler.h>

// Handler principal invocado por el dispatcher
void GetHandler::handle(const HandlerContext &ctx) {

    // Validación de existencia
    if (!ctx.sensor && !ctx.actuator) {
        if (Constants::DEBUG)
            Serial.println("[GetHandler] ID no encontrado en sensores ni actuadores.");
        return;
    }

    // Selección según tipo
    if (ctx.sensor) {
        handleSensorGet(ctx);
    } else if (ctx.actuator) {
        handleActuatorGet(ctx);
    }
}

// GET aplicado sobre un sensor
void GetHandler::handleSensorGet(const HandlerContext &ctx) {

    if (!(*ctx.json)["requester"]) {
        if (Constants::DEBUG)
            Serial.println("[GetHandler] GET sensor inválido: falta requester.");
        return;
    }

    String requester = (*ctx.json)["requester"].as<String>();

    // Construcción del JSON de respuesta
    JsonDocument response;
    response["value"] = ctx.sensor->getValue();
    response["units"] = ctx.sensor->getUnits();

    // NUEVO: incluir estado enabled
    response["enabled"] = ctx.sensor->isEnabled();

    response["requester"] = requester;

    publishResponse(ctx, response);

    if (Constants::DEBUG) {
        Serial.printf(
            "[GetHandler] RESPONSE sensor => %s / %s (%f %s, enabled=%s)\n",
            ctx.sensor->name(),
            ctx.sensor->location(),
            ctx.sensor->getValue(),
            ctx.sensor->getUnits(),
            ctx.sensor->isEnabled() ? "true" : "false"
        );
    }
}

// GET aplicado sobre un actuador
void GetHandler::handleActuatorGet(const HandlerContext &ctx) {

    if (!ctx.json->containsKey("requester")) {
        if (Constants::DEBUG)
            Serial.println("[GetHandler] GET actuador inválido: falta requester.");
        return;
    }

    String requester = (*ctx.json)["requester"].as<String>();

    // Construcción del JSON de respuesta
    JsonDocument response;
    response["state"] = ctx.actuator->stateString();
    response["requester"] = requester;

    publishResponse(ctx, response);

    if (Constants::DEBUG) {
        Serial.printf(
            "[GetHandler] RESPONSE actuator => %s / %s (state=%s)\n",
            ctx.actuator->name(),
            ctx.actuator->location(),
            ctx.actuator->stateString().c_str()
        );
    }
}

// Publicar tópico response según formato oficial
void GetHandler::publishResponse(const HandlerContext &ctx, const JsonDocument &response) {

    String topic = "response/";
    topic += Constants::Mqtt::DEVICE_ID;
    topic += "/";
    topic += ctx.info.type;
    topic += "/";
    topic += ctx.info.id;

    String payload;
    serializeJson(response, payload);

    ctx.mqtt->publish(topic, payload);

    if (Constants::DEBUG) {
        Serial.printf("[GetHandler] Publicado RESPONSE => %s : %s\n",
                      topic.c_str(), payload.c_str());
    }
}
