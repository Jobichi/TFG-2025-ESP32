// ======================= ServoMotor360.h =======================
#pragma once

#include <Arduino.h>
#include <Devices/Actuators/Actuator.h>
#include <ESP32Servo.h>

struct ServoConfig{
    int channelHint = 0;
    const char *friendlyName = "Servo360";
    const char *location = "room-name";
};

class ServoMotor360 : public Actuator {
public:
    explicit ServoMotor360(
        uint8_t pin,
        const ServoConfig &cfg = {}
    );

    // Ciclo de vida
    bool begin() override;
    void loop() override;

    // Identificación
    const char* name() const override { return friendlyName_.c_str(); }
    const char* location() const override { return location_.c_str(); }

    // Control genérico (heredado)
    bool isActive() const override { return active_; }

    // Control específico del servo continuo
    void forward(int speed = 100);
    void backward(int speed = 100);
    void stop();

    // Configuración (calibración)
    void setNeutralUs(int neutralUs) { neutralUs_ = neutralUs; }
    void setDeadbandUs(int deadbandUs) { deadbandUs_ = deadbandUs; }

    // Estado textual (para announces)
    String stateString() const override;

    // Interfaz de comandos (ej: "FORWARD", "FORWARD:60", "OPEN 40", "STOP")
    bool applyCommand(const char* command) override;

private:
    uint8_t pin_;
    int channelHint_;
    int attachedChannel_{0};

    String friendlyName_;
    String location_;

    bool active_{false};

    enum Direction { STOPPED, FORWARD, BACKWARD };
    Direction dir_{STOPPED};

    Servo servo_;

    // Parámetros PWM típicos (ajustables)
    static constexpr int SERVO_MIN_US = 1000;   // reverse max
    static constexpr int SERVO_MAX_US = 2000;   // forward max

    int neutralUs_{1500};       // parar (calibrable)
    int deadbandUs_{10};        // banda muerta alrededor del neutro (calibrable)

private:
    static int clampSpeed(int speed);
    int speedToPulse(Direction d, int speed) const;

    // Parsing simple de "CMD", "CMD:NN", "CMD NN"
    static bool parseCommand(const char* in, String& cmdOut, int& speedOut);
};