#include "Actuators/ServoMotor360.h"

ServoMotor360::ServoMotor360(uint8_t pin, int channel, const char* friendlyName, const char* location)
    : pin_(pin), channel_(channel), friendlyName_(friendlyName), location_(location) {}

bool ServoMotor360::begin() {
    // Adjuntar servo y configurar PWM
    servo_.setPeriodHertz(50);
    servo_.attach(pin_, SERVO_MIN_US, SERVO_MAX_US);
    stop();  // arranca parado

    #if DEBUG
        Serial.printf("[SERVO] %s inicializado en pin %d\n", friendlyName_.c_str(), pin_);
    #endif

    return true;
}

void ServoMotor360::loop() {
    // Si tuvieras lógica de tiempo o feedback, iría aquí
}

void ServoMotor360::on() {
    forward();
}

void ServoMotor360::off() {
    stop();
}

void ServoMotor360::forward(int speed) {
    speed = constrain(speed, 0, 100);
    int us = map(speed, 0, 100, SERVO_STOP_US, SERVO_MAX_US);
    servo_.writeMicroseconds(us);
    dir_ = FORWARD;
    active_ = true;

    #if DEBUG
        Serial.printf("[SERVO] %s → FORWARD (%d%%)\n", friendlyName_.c_str(), speed);
    #endif
}

void ServoMotor360::backward(int speed) {
    speed = constrain(speed, 0, 100);
    int us = map(speed, 0, 100, SERVO_STOP_US, SERVO_MIN_US);
    servo_.writeMicroseconds(us);
    dir_ = BACKWARD;
    active_ = true;

    #if DEBUG
        Serial.printf("[SERVO] %s → BACKWARD (%d%%)\n", friendlyName_.c_str(), speed);
    #endif
}

void ServoMotor360::stop() {
    servo_.writeMicroseconds(SERVO_STOP_US);
    dir_ = STOPPED;
    active_ = false;

    #if DEBUG
        Serial.printf("[SERVO] %s → STOP\n", friendlyName_.c_str());
    #endif
}

String ServoMotor360::stateString() const {
    switch (dir_) {
        case FORWARD:  return "UP";
        case BACKWARD: return "DOWN";
        default:       return "STOP";
    }
}

bool ServoMotor360::applyCommand(const char* command) {
    if (strcasecmp(command, "UP") == 0 || strcasecmp(command, "OPEN") == 0) {
        forward();
        return true;
    }
    if (strcasecmp(command, "DOWN") == 0 || strcasecmp(command, "CLOSE") == 0) {
        backward();
        return true;
    }
    if (strcasecmp(command, "STOP") == 0) {
        stop();
        return true;
    }

    #if DEBUG
        Serial.printf("[SERVO] Comando desconocido: %s\n", command);
    #endif

    return false;
}
