#include "Actuators/ServoMotor360.h"
#include "configCredentials.h"

ServoMotor360::ServoMotor360(
    uint8_t pin, int channel, const char* friendlyName
) : pin_(pin), channel_(channel), friendlyName_(friendlyName) {}

bool ServoMotor360::begin() {
    ledcSetup(channel_, SERVO_FREQ, SERVO_RES);
    ledcAttachPin(pin_, channel_);
    stop();
    return true;
}

void ServoMotor360::loop() {}

void ServoMotor360::on() {
    forward(100);
}

void ServoMotor360::off() {
    stop();
}

void ServoMotor360::forward(int speed) {
    speed = constrain(speed, 0, 100);
    int us = PULSE_STOP - map(speed, 0, 100, 0, PULSE_RANGE);
    ledcWrite(channel_, usToDuty(us));
    active_ = (speed > 0);

    #if DEBUG
        Serial.printf("[%s] FORWARD %d%%\n", friendlyName_, speed);
    #endif
}

void ServoMotor360::backward(int speed) {
    speed = constrain(speed, 0, 100);
    int us = PULSE_STOP + map(speed, 0, 100, 0, PULSE_RANGE);
    ledcWrite(channel_, usToDuty(us));
    active_ = (speed > 0);

    #if DEBUG
        Serial.printf("[%s] BACKWARD %d%%\n", friendlyName_, speed);
    #endif
}

void ServoMotor360::stop() {
    ledcWrite(channel_, usToDuty(PULSE_STOP));
    active_ = false;

    #if DEBUG
        Serial.printf("[%s] STOP\n", friendlyName_);
    #endif
}

int ServoMotor360::usToDuty(int us) const {
    return (int)((float)us / (1000000.0 / SERVO_FREQ) * (1 << SERVO_RES));
}