#pragma once
#include "Actuators/ActuatorBase.h"

class ServoMotor360 : public ActuatorBase {
    public:
        explicit ServoMotor360(
            uint8_t pin,
            int channel = 0,
            const char* friendlyName = "Servo360",
            const char* location = "room-name"
        );

        bool begin() override;
        void loop() override;
        const char* name() const override { return friendlyName_.c_str(); }
        const char* location() const override { return location_.c_str(); }

        void on() override;
        void off() override;
        bool isActive() const override { return active_; }

        // Métodos específicos para el servo:
        void forward(int speed = 100);
        void backward(int speed = 100);
        void stop();

        String stateString() override;

    private:
        uint8_t pin_;
        int channel_;
        String friendlyName_;
        String location_;
        bool active_{false};

        // Dirección del movimiento
        enum Direction { 
            STOPPED, 
            FORWARD, 
            BACKWARD 
        };
        
        Direction dir_{STOPPED};

        // Parámetros PWM para SG90 continuo
        static constexpr int SERVO_FREQ = 50;
        static constexpr int SERVO_RES  = 16;
        static constexpr int PULSE_STOP = 1500;
        static constexpr int PULSE_RANGE = 500;

        int usToDuty(int us) const;
};
