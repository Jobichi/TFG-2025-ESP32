// include/Logic/NodeApp.h
#pragma once

#include <Arduino.h>
#include <functional>
#include <memory>
#include <map>

#include <Logic/DeviceRegistry.h>
#include <Logic/AlertEngine.h>

#include <Connectivity/wifi/WifiManager.h>
#include <Connectivity/mqtt/MqttManager.h>
#include <Connectivity/mqtt/Dispatcher.h>

#include <Connectivity/mqtt/handlers/getHandler.h>
#include <Connectivity/mqtt/handlers/setHandler.h>

#include <Connectivity/mqtt/publishers/AnnouncePublisher.h>
#include <Connectivity/mqtt/publishers/UpdatePublisher.h>
#include <Connectivity/mqtt/publishers/AlertPublisher.h>

class NodeApp {
public:
    struct Options {
        bool announceOnBoot;
        bool periodicUpdates;
        uint32_t updatePeriodMs;

        bool waitWifi;
        bool waitMqtt;
        uint32_t waitStepMs;

        Options()
            : announceOnBoot(true),
              periodicUpdates(true),
              updatePeriodMs(8000),
              waitWifi(true),
              waitMqtt(true),
              waitStepMs(150) {}
    };

    typedef std::function<void(DeviceRegistry&)> DeviceSetupFn;
    typedef std::function<void(AlertEngine&)> AlertSetupFn;

    NodeApp() : mqtt_(wifi_) {}

    DeviceRegistry& devices() { return registry_; }

    // Overload “simple” (usa Options por defecto)
    void begin(const DeviceSetupFn& deviceSetup,
               const AlertSetupFn& alertSetup = AlertSetupFn()) {
        Options opt;
        begin(deviceSetup, opt, alertSetup);
    }

    // Overload “completo” (sin defaults problemáticos)
    void begin(const DeviceSetupFn& deviceSetup,
               const Options& opt,
               const AlertSetupFn& alertSetup) {
        wifi_.begin();
        mqtt_.begin();

        if (deviceSetup) deviceSetup(registry_);

        for (std::map<int, Sensor*>::iterator it = registry_.sensors().begin(); it != registry_.sensors().end(); ++it) {
            if (it->second) it->second->begin();
        }
        for (std::map<int, Actuator*>::iterator it = registry_.actuators().begin(); it != registry_.actuators().end(); ++it) {
            if (it->second) it->second->begin();
        }

        dispatcher_.reset(new Dispatcher(registry_.sensors(), registry_.actuators(), mqtt_));
        dispatcher_->registerHandler("get", [&](const HandlerContext& ctx){ getHandler_.handle(ctx); });
        dispatcher_->registerHandler("set", [&](const HandlerContext& ctx){ setHandler_.handle(ctx); });

        mqtt_.addSubHandler("get/#", [&](const String& topic, const String& payload){ dispatcher_->route(topic, payload); });
        mqtt_.addSubHandler("set/#", [&](const String& topic, const String& payload){ dispatcher_->route(topic, payload); });

        announcePublisher_.reset(new AnnouncePublisher(registry_.sensors(), registry_.actuators(), mqtt_));
        updatePublisher_.reset(new UpdatePublisher(registry_.sensors(), registry_.actuators(), mqtt_));
        alertPublisher_.reset(new AlertPublisher(registry_.sensors(), registry_.actuators(), mqtt_));

        alertEngine_.reset(new AlertEngine(registry_, *alertPublisher_));

        if (opt.waitWifi) {
            while (!wifi_.isConnected()) {
                wifi_.loop();
                delay(opt.waitStepMs);
            }
        }

        if (opt.waitMqtt) {
            while (!mqtt_.isConnected()) {
                mqtt_.loop();
                delay(opt.waitStepMs);
            }
        }

        if (opt.announceOnBoot && announcePublisher_) {
            announcePublisher_->publishAll();
        }

        periodicUpdateEnabled_ = opt.periodicUpdates;
        updatePeriodMs_ = opt.updatePeriodMs;
        lastUpdateMs_ = millis();

        if (updatePublisher_) updatePublisher_->setEnabled(true);

        if (alertSetup && alertEngine_) {
            alertSetup(*alertEngine_);
        }
    }

    void loop() {
        wifi_.loop();
        mqtt_.loop();

        for (std::map<int, Sensor*>::iterator it = registry_.sensors().begin(); it != registry_.sensors().end(); ++it) {
            if (it->second) it->second->loop();
        }
        for (std::map<int, Actuator*>::iterator it = registry_.actuators().begin(); it != registry_.actuators().end(); ++it) {
            if (it->second) it->second->loop();
        }

        if (alertEngine_) alertEngine_->loop();

        if (periodicUpdateEnabled_ && updatePublisher_) {
            const uint32_t now = millis();
            if (now - lastUpdateMs_ >= updatePeriodMs_) {
                lastUpdateMs_ = now;
                updatePublisher_->publishAll();
            }
        }
    }

private:
    WifiManager wifi_;
    MqttManager mqtt_;

    DeviceRegistry registry_;

    std::unique_ptr<Dispatcher> dispatcher_;
    GetHandler getHandler_;
    SetHandler setHandler_;

    std::unique_ptr<AnnouncePublisher> announcePublisher_;
    std::unique_ptr<UpdatePublisher> updatePublisher_;
    std::unique_ptr<AlertPublisher> alertPublisher_;
    std::unique_ptr<AlertEngine> alertEngine_;

    bool periodicUpdateEnabled_ = true;
    uint32_t updatePeriodMs_ = 8000;
    uint32_t lastUpdateMs_ = 0;
};
