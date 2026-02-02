// ======================= ServoMotor360.cpp =======================
#include <Devices/Actuators/ServoMotor360.h>

ServoMotor360::ServoMotor360(
    uint8_t pin,
    const ServoConfig &cfg
)
    : pin_(pin),
      channelHint_(cfg.channelHint),
      friendlyName_(cfg.friendlyName),
      location_(cfg.location) {}

bool ServoMotor360::begin() {
    servo_.setPeriodHertz(50);

    // Reserva un único timer (0..3) usando el hint si viene bien
    int t = channelHint_;
    if (t < 0) t = 0;
    if (t > 3) t = 3;

    ESP32PWM::allocateTimer(t);

    if (servo_.attached()) {
        servo_.detach();
        delay(10);
    }

    attachedChannel_ = servo_.attach(pin_, SERVO_MIN_US, SERVO_MAX_US);
    if (attachedChannel_ >= 0) {
        servo_.writeMicroseconds(neutralUs_);
        active_ = false;
        dir_ = STOPPED;
        return true;
    }

    Serial.println("[ServoMotor360] Error attaching servo (LEDC resources?)");
    return false;
}

void ServoMotor360::loop() {
    // Vacío por defecto
}

void ServoMotor360::forward(int speed) {
    speed = clampSpeed(speed);
    const int pulse = speedToPulse(FORWARD, speed);
    servo_.writeMicroseconds(pulse);

    active_ = (speed > 0);
    dir_ = active_ ? FORWARD : STOPPED;
}

void ServoMotor360::backward(int speed) {
    speed = clampSpeed(speed);
    const int pulse = speedToPulse(BACKWARD, speed);
    servo_.writeMicroseconds(pulse);

    active_ = (speed > 0);
    dir_ = active_ ? BACKWARD : STOPPED;
}

void ServoMotor360::stop() {
    servo_.writeMicroseconds(neutralUs_);
    active_ = false;
    dir_ = STOPPED;
}

String ServoMotor360::stateString() const {
    switch (dir_) {
        case FORWARD:  return "FORWARD";
        case BACKWARD: return "BACKWARD";
        default:       return "STOP";
    }
}

bool ServoMotor360::applyCommand(const char* command) {
    if (!command || command[0] == '\0') return false;

    String cmd;
    int speed = 100;
    if (!parseCommand(command, cmd, speed)) return false;

    cmd.toUpperCase();

    if (cmd == "FORWARD" || cmd == "OPEN" || cmd == "UP") {
        forward(speed);
        return true;
    }

    if (cmd == "BACKWARD" || cmd == "CLOSE" || cmd == "DOWN") {
        backward(speed);
        return true;
    }

    if (cmd == "STOP") {
        stop();
        return true;
    }

    return false;
}

int ServoMotor360::clampSpeed(int speed) {
    return constrain(speed, 0, 100);
}

int ServoMotor360::speedToPulse(Direction d, int speed) const {
    // Si speed es 0, nos quedamos en neutro (parado)
    if (speed <= 0) return neutralUs_;

    // Banda muerta alrededor del neutro para evitar drift
    const int usableFwd = (SERVO_MAX_US - (neutralUs_ + deadbandUs_));
    const int usableRev = ((neutralUs_ - deadbandUs_) - SERVO_MIN_US);

    if (d == FORWARD) {
        const int delta = (usableFwd * speed) / 100;
        return constrain(neutralUs_ + deadbandUs_ + delta, SERVO_MIN_US, SERVO_MAX_US);
    }

    if (d == BACKWARD) {
        const int delta = (usableRev * speed) / 100;
        return constrain(neutralUs_ - deadbandUs_ - delta, SERVO_MIN_US, SERVO_MAX_US);
    }

    return neutralUs_;
}

bool ServoMotor360::parseCommand(const char* in, String& cmdOut, int& speedOut) {
    // Acepta "CMD", "CMD:NN" y "CMD NN"
    String s(in);
    s.trim();
    if (s.length() == 0) return false;

    int sep = s.indexOf(':');
    if (sep < 0) sep = s.indexOf(' ');

    if (sep < 0) {
        cmdOut = s;
        speedOut = 100;
        return true;
    }

    cmdOut = s.substring(0, sep);
    cmdOut.trim();

    String rhs = s.substring(sep + 1);
    rhs.trim();

    if (rhs.length() == 0) {
        speedOut = 100;
        return true;
    }

    speedOut = constrain(rhs.toInt(), 0, 100);
    return true;
}
