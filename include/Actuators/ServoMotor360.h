#pragma once
#include "Actuators/ActuatorBase.h"
#include <ESP32Servo.h>

class ServoMotor360 : public ActuatorBase {
public:
    explicit ServoMotor360(
        uint8_t pin,
        int channel = 0,
        const char* friendlyName = "Servo360",
        const char* location = "room-name"
    );

    // Ciclo de vida
    bool begin() override;
    void loop() override;

    // Identificación
    const char* name() const override { return friendlyName_.c_str(); }
    const char* location() const override { return location_.c_str(); }

    // Control genérico (heredado)
    void on() override;   // podría equivaler a "forward"
    void off() override;  // detiene el servo
    bool isActive() const override { return active_; }

    // Control específico del servo
    void forward(int speed = 100);   // abrir / subir
    void backward(int speed = 100);  // cerrar / bajar
    void stop();

    // Estado textual (para announces)
    String stateString() const override;

    // Interfaz de comandos MQTT
    bool applyCommand(const char* command) override;

private:
    uint8_t pin_;
    int channel_;
    String friendlyName_;
    String location_;
    bool active_{false};

    enum Direction {
        STOPPED,
        FORWARD,
        BACKWARD
    };
    Direction dir_{STOPPED};

    Servo servo_;  // objeto de la librería ESP32Servo

    // Parámetros PWM para SG90 continuo
    static constexpr int SERVO_MIN_US = 1000;  // full reverse
    static constexpr int SERVO_MAX_US = 2000;  // full forward
    static constexpr int SERVO_STOP_US = 1500; // stop
};