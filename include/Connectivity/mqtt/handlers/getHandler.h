#pragma once
#include <Connectivity/mqtt/Dispatcher.h>
#include <ArduinoJson.h>
#include <Constants.h>

class GetHandler {
    public:
        void handle(const HandlerContext &ctx);

    private:
        void handleSensorGet(const HandlerContext &ctx);
        void handleActuatorGet(const HandlerContext &ctx);
        void publishResponse(const HandlerContext &ctx, const JsonDocument &response);
};