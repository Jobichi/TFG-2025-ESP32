#pragma once
#include <Connectivity/mqtt/Dispatcher.h>
#include <ArduinoJson.h>
#include <Constants.h>

class SetHandler {
    public:
        // Método que ejecuta el dispatcher
        void handle(const HandlerContext &ctx);

    private:
        void handleSensorSet(const HandlerContext &ctx);
        void handleActuatorSet(const HandlerContext &ctx);
        void publishResponse(const HandlerContext &ctx, const JsonDocument &response);
};