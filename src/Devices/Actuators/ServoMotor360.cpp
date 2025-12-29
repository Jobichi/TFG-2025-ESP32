#include <Devices/Actuators/ServoMotor360.h>
#include <Arduino.h>

ServoMotor360::ServoMotor360(
    uint8_t pin,
    int channel,
    const char* friendlyName,
    const char* location
)
    : pin_(pin),
      channel_(channel),
      friendlyName_(friendlyName),
      location_(location),
      active_(false),
      dir_(STOPPED)
{}

// Inicialización del servo continuo
bool ServoMotor360::begin() {
    servo_.setPeriodHertz(50);  // SG90 usa 50 Hz

    if (!servo_.attach(pin_, SERVO_MIN_US, SERVO_MAX_US)) {
        Serial.println("[ServoMotor360] Error attaching servo");
        return false;
    }

    stop(); // estado seguro
    return true;
}

// Si necesitas animaciones, timers, etc., van aquí
void ServoMotor360::loop() {
    // Vacío por defecto
}

// Movimiento hacia delante
void ServoMotor360::forward(int speed) {
    if (speed < 0) speed = 0;
    if (speed > 100) speed = 100;

    int pulse = map(speed, 0, 100, SERVO_STOP_US, SERVO_MAX_US);
    servo_.writeMicroseconds(pulse);

    active_ = true;
    dir_ = FORWARD;
}

// Movimiento hacia atrás
void ServoMotor360::backward(int speed) {
    if (speed < 0) speed = 0;
    if (speed > 100) speed = 100;

    int pulse = map(speed, 0, 100, SERVO_STOP_US, SERVO_MIN_US);
    servo_.writeMicroseconds(pulse);

    active_ = true;
    dir_ = BACKWARD;
}

// Detener movimiento
void ServoMotor360::stop() {
    servo_.writeMicroseconds(SERVO_STOP_US);
    active_ = false;
    dir_ = STOPPED;
}

// Estado textual uniforme (para MQTT)
String ServoMotor360::stateString() const {
    switch (dir_) {
        case FORWARD:  return "FORWARD";
        case BACKWARD: return "BACKWARD";
        default:       return "STOP";
    }
}

// Interpretación de comandos SET recibidos vía MQTT
bool ServoMotor360::applyCommand(const char* command) {

    if (strcmp(command, "FORWARD") == 0 ||
        strcmp(command, "OPEN") == 0 ||
        strcmp(command, "UP") == 0) {
        forward();
        return true;
    }

    if (strcmp(command, "BACKWARD") == 0 ||
        strcmp(command, "CLOSE") == 0 ||
        strcmp(command, "DOWN") == 0) {
        backward();
        return true;
    }

    if (strcmp(command, "STOP") == 0) {
        stop();
        return true;
    }

    return false;
}
