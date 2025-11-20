#include <Connectivity/mqtt/handlers/setHandler.h>
#include <Connectivity/mqtt/MqttManager.h>

// Handler principal invocado por el dispatcher
void SetHandler::handle(const HandlerContext &ctx){

    // Validación de existencia
    if(!ctx.sensor && !ctx.actuator){
        if (Constants::DEBUG)
            Serial.println("[SetHandler] ID no encontrado en sensores ni actuadores.");
        return;
    }

    // Redirección en función del type
    if(ctx.sensor){
        handleSensorSet(ctx);
    } else if (ctx.actuator){
        handleActuatorSet(ctx);
    }
}

// SET aplicado sobre un sensor (habilitado/deshabilitado)
void SetHandler::handleSensorSet(const HandlerContext &ctx){
    if (!(*ctx.json)["enable"].is<bool>()){
        if (Constants::DEBUG)
            Serial.println("[SetHandler] Payload SET para sensor inválido.");
        return;
    }

    bool enable = (*ctx.json)["enable"].as<bool>();
    ctx.sensor->setEnabled(enable); // Implementado en clase sensor

    // Construcción de respuesta JSON
    JsonDocument response;
    response["enable"] = enable;
    response["requester"] = (*ctx.json)["requester"] | "unknown";

    publishResponse(ctx, response);
}

// SET aplicado sobre un actuador
void SetHandler::handleActuatorSet(const HandlerContext &ctx){
    if(!ctx.json->containsKey("state")){
        if (Constants::DEBUG)
            Serial.println("[SetHandler] Payload SET para actuador inválido.");
        return;
    }

    String newState = (*ctx.json)["state"].as<String>();
    ctx.actuator->applyCommand(newState.c_str()); // Implementado en clase actuador

    // Construir respuesta JSON
    JsonDocument response;
    response["state"] = newState;
    response["requester"] = (*ctx.json)["requester"] | "unknown";

    publishResponse(ctx, response);
}

// Publicar un tópico response según el formato esperado
void SetHandler::publishResponse(const HandlerContext &ctx, const JsonDocument &response){
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