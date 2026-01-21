// include/Logic/DeviceRegistry.h
#pragma once

#include <Arduino.h>

#include <map>
#include <vector>
#include <memory>
#include <utility>

// Bases
#include <Devices/Sensors/Sensor.h>
#include <Devices/Actuators/Actuator.h>

// Sensores (según tu repo)
#include <Devices/Sensors/FireSensor.h>
#include <Devices/Sensors/WaterLeakSensor.h>
#include <Devices/Sensors/DoorSensor.h>
#include <Devices/Sensors/LdrSensor.h>
#include <Devices/Sensors/MqSensor.h>
#include <Devices/Sensors/PirSensor.h>
#include <Devices/Sensors/Ds18b20Sensor.h>
#include <Devices/Sensors/Bmp280Sensor.h>
#include <Devices/Sensors/SoilMoistureSensor.h>
#include <Devices/Sensors/Dht22Base.h>
#include <Devices/Sensors/Dht22TemperatureSensor.h>
#include <Devices/Sensors/Dht22HumiditySensor.h>

// Actuadores (según tu repo)
#include <Devices/Actuators/Buzzer.h>
#include <Devices/Actuators/Relay.h>
#include <Devices/Actuators/ServoMotor360.h>
#include <Devices/Actuators/BuzzerWithFeedback.h>
#include <Devices/Actuators/ServoMotor360Endstops.h>


class DeviceRegistry {
public:
    // ---------- API genérica (si quieres control total) ----------
    template <typename TSensor, typename... Args>
    TSensor& addSensor(int id, Args&&... args) {
        TSensor* ptr = new TSensor(std::forward<Args>(args)...);
        ownedSensors_.emplace_back(ptr);
        sensors_[id] = ptr;
        return *ptr;
    }

    template <typename TActuator, typename... Args>
    TActuator& addActuator(int id, Args&&... args) {
        TActuator* ptr = new TActuator(std::forward<Args>(args)...);
        ownedActuators_.emplace_back(ptr);
        actuators_[id] = ptr;
        return *ptr;
    }

    // ---------- Helpers por tipo (evitan errores por parámetros intermedios) ----------

    // FireSensor(uint8_t pin, bool activeHigh=false, const char* name, const char* location)
    FireSensor& addFire(int id, uint8_t pin, const char* name, const char* location, bool activeHigh = false) {
        FireConfig cfg;
        cfg.friendlyName = name;
        cfg.location = location;
        cfg.activeHigh = activeHigh;
        return addSensor<FireSensor>(id, pin, cfg);
    }

    // WaterLeakSensor(uint8_t pin, bool activeHigh=true, const char* name, const char* location)
    WaterLeakSensor& addWaterLeak(int id, uint8_t pin, const char* name, const char* location, bool activeHigh = true) {
        WaterLeakConfig cfg;
        cfg.friendlyName = name;
        cfg.location = location;
        cfg.activeHigh = activeHigh;
        return addSensor<WaterLeakSensor>(id, pin, cfg);
    }

    // DoorSensor(uint8_t pin, bool activeHigh=true, const char* name, const char* location)
    DoorSensor& addDoor(int id, uint8_t pin, const char* name, const char* location, bool activeHigh = true) {
        DoorSensorConfig cfg;
        cfg.friendlyName = name;
        cfg.location = location;
        cfg.activeHigh = activeHigh;
        return addSensor<DoorSensor>(id, pin, cfg);
    }

    // LdrSensor(uint8_t pin, unsigned long readPeriodMs=200, const char* name, const char* location)
    LdrSensor& addLdr(int id, uint8_t pin, const char* name, const char* location, unsigned long periodMs = 200UL) {
        LdrConfig cfg;
        cfg.friendlyName = name;
        cfg.location = location;
        cfg.readPeriodMs = periodMs;
        return addSensor<LdrSensor>(id, pin, cfg);
    }

    // MqSensor(uint8_t pin, unsigned long readPeriodMs=1000, float thresholdVoltage=0.0, const char* name, const char* location)
    MqSensor& addMq(int id, uint8_t pin, const char* name, const char* location,
                    unsigned long periodMs = 1000UL, float thresholdV = 0.0f) {
        MqConfig cfg;
        cfg.friendlyName = name;
        cfg.location = location;
        cfg.readPeriodMs = periodMs;
        cfg.thresholdVoltage = thresholdV;
        return addSensor<MqSensor>(id, pin, cfg);
    }

    // PirSensor(uint8_t pin, bool activeHigh=true, bool usePulldown=false, unsigned long warmupMs=60000, unsigned long stableMs=80, const char* name, const char* location)
    PirSensor& addPir(int id, uint8_t pin, const char* name, const char* location,
                      bool activeHigh = true, bool usePulldown = false,
                      unsigned long warmupMs = 60000UL, unsigned long stableMs = 80UL) {
        PirConfig cfg;
        cfg.activeHigh = activeHigh;
        cfg.friendlyName = name;
        cfg.usePulldown = usePulldown;
        cfg.warmupMs = warmupMs;
        cfg.stableMs = stableMs;
        cfg.location = location;
        return addSensor<PirSensor>(id, pin, cfg);
    }

    // Ds18b20Sensor(uint8_t pin, unsigned long readPeriodMs=2000, const char* name, const char* location)
    Ds18b20Sensor& addDs18b20(int id, uint8_t pin, const char* name, const char* location, unsigned long periodMs = 2000UL) {
        Ds18b20Config cfg;
        cfg.friendlyName = name;
        cfg.location = location;
        cfg.readPeriodMs = periodMs;
        return addSensor<Ds18b20Sensor>(id, pin, cfg);
    }

    // Bmp280Sensor(uint8_t i2cAddress=0x76, unsigned long readPeriodMs=2000, const char* name, const char* location)
    Bmp280Sensor& addBmp280(int id, const char* name, const char* location,
                            uint8_t i2cAddress = 0x76, unsigned long periodMs = 2000UL) {
        Bmp280Config cfg;
        cfg.i2cAddress = i2cAddress;
        cfg.readPeriodMs = periodMs;
        cfg.friendlyName = name;
        cfg.location = location;
        return addSensor<Bmp280Sensor>(id, cfg);
    }


    // SoilMoistureSensor(uint8_t pin, unsigned long readPeriodMs=2000, int rawDry=3100, int rawWet=2150, int thresholdPercent=40, int hysteresisPercent=5, const char* name, const char* location)
    SoilMoistureSensor& addSoilMoisture(int id, uint8_t pin, const char* name, const char* location, unsigned long periodMs = 2000UL, int rawDry = 3100, int rawWet = 2150,  int thresholdPercent = 40, int hysteresisPercent = 5) {
        SoilMoistureConfig cfg;
        cfg.friendlyName = name;
        cfg.location = location;
        cfg.readPeriodMs = periodMs;
        cfg.rawDry = rawDry;
        cfg.rawWet = rawWet;
        cfg.thresholdPercent = thresholdPercent;
        cfg.hysteresisPercent = hysteresisPercent;
        return addSensor<SoilMoistureSensor>(
            id, pin, cfg);
    }

    // ---- DHT22: necesitas un driver compartido Dht22Base ----
    Dht22Base& createDht22Driver(uint8_t pin) {
        Dht22Base* drv = new Dht22Base(pin);
        ownedDhtDrivers_.emplace_back(drv);
        return *drv;
    }

    // Dht22TemperatureSensor(Dht22Base& driver, unsigned long readPeriodMs, const char* name, const char* location)
    Dht22TemperatureSensor& addDht22Temperature(int id, Dht22Base& driver, unsigned long periodMs,
                                                const char* name = "Temp", const char* location = "room-name") {
        Dht22TemperatureConfig cfg;
        cfg.readPeriodMs = periodMs;
        cfg.friendlyName = name;
        cfg.location = location;
        return addSensor<Dht22TemperatureSensor>(id, driver, cfg);
    }

    // Dht22HumiditySensor(Dht22Base& driver, unsigned long readPeriodMs, const char* name, const char* location)
    Dht22HumiditySensor& addDht22Humidity(int id, Dht22Base& driver, unsigned long periodMs,
                                          const char* name = "Humidity", const char* location = "room-name") {
        Dht22HumidityConfig cfg;
        cfg.readPeriodMs = periodMs;
        cfg.friendlyName = name;
        cfg.location = location;
        return addSensor<Dht22HumiditySensor>(id, driver, cfg);
    }

    // Helper cómodo: crea driver y registra los dos sensores con IDs distintos
    void addDht22Pair(int tempId, int humId, uint8_t pin,
                      const char* location,
                      unsigned long periodMs = 2000UL,
                      const char* tempName = "Temp",
                      const char* humName = "Humidity") {
        Dht22Base& drv = createDht22Driver(pin);
        addDht22Temperature(tempId, drv, periodMs, tempName, location);
        addDht22Humidity(humId, drv, periodMs, humName, location);
    }

    // ---------- Actuadores ----------
    // Buzzer(uint8_t pin, const char* name, const char* location)
    Buzzer& addBuzzer(int id, uint8_t pin, const char* name, const char* location) {
        BuzzerConfig cfg;
        cfg.friendlyName = name;
        cfg.location = location;
        return addActuator<Buzzer>(id, pin, cfg);
    }
    
    // BuzzerWithFeedback(uint8_t outPin, uint8_t feedbackPin, cfg)
    BuzzerWithFeedback& addBuzzerWithFeedback(int id,
                                            uint8_t outPin,
                                            uint8_t feedbackPin,
                                            const char* name,
                                            const char* location,
                                            bool useInternalPulldown = true,
                                            uint32_t debounceMs = 40,
                                            bool serialLog = true) {
        BuzzerWithFeedbackConfig cfg;
        cfg.friendlyName = name;
        cfg.location = location;
        cfg.useInternalPulldown = useInternalPulldown;
        cfg.debounceMs = debounceMs;
        cfg.serialLog = serialLog;

        return addActuator<BuzzerWithFeedback>(id, outPin, feedbackPin, cfg);
    }


    // Relay(uint8_t pin, bool activeLow=true, const char* name, const char* location)
    Relay& addRelay(int id, uint8_t pin, const char* name, const char* location, bool activeLow = true) {
        RelayConfig cfg;
        cfg.friendlyName = name;
        cfg.activeLow = activeLow;
        cfg.location = location;
        return addActuator<Relay>(id, pin, cfg);
    }

    // ServoMotor360(uint8_t pin, int channel=0, const char* name, const char* location)
    ServoMotor360& addServo360(int id, uint8_t pin, const char* name, const char* location, int channel = 0) {
        ServoConfig cfg;
        cfg.friendlyName = name;
        cfg.location = location;
        cfg.channelHint = channel;
        return addActuator<ServoMotor360>(id, pin, cfg);
    }
    
    // ServoMotor360Endstops(uint8_t servoPin, servoCfg, endCfg)
    ServoMotor360Endstops& addServo360Endstops(int id,
                                            uint8_t servoPin,
                                            uint8_t pinOpen,
                                            uint8_t pinClose,
                                            const char* name,
                                            const char* location,
                                            int channel = 0,
                                            bool activeLow = true,
                                            uint32_t debounceMs = 30,
                                            uint32_t maxRunMs = 15000) {
        ServoConfig scfg;
        scfg.friendlyName = name;
        scfg.location = location;
        scfg.channelHint = channel;

        EndstopsConfig ecfg;
        ecfg.pinOpen = pinOpen;
        ecfg.pinClose = pinClose;
        ecfg.activeLow = activeLow;
        ecfg.debounceMs = debounceMs;
        ecfg.maxRunMs = maxRunMs;

        return addActuator<ServoMotor360Endstops>(id, servoPin, scfg, ecfg);
    }

    // ---------- Acceso a mapas (compatibles con tu Dispatcher/Publishers) ----------
    std::map<int, Sensor*>& sensors() { return sensors_; }
    std::map<int, Actuator*>& actuators() { return actuators_; }

private:
    std::map<int, Sensor*> sensors_;
    std::map<int, Actuator*> actuators_;

    std::vector<std::unique_ptr<Sensor>> ownedSensors_;
    std::vector<std::unique_ptr<Actuator>> ownedActuators_;

    // DHT22 drivers compartidos
    std::vector<std::unique_ptr<Dht22Base>> ownedDhtDrivers_;
};


