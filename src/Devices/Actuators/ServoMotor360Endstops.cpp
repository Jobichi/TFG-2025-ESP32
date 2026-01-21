#include <Devices/Actuators/ServoMotor360Endstops.h>

void ServoMotor360Endstops::DebouncedInput::begin() {
    if (pin == 255) return;
    pinMode(pin, INPUT_PULLUP);
    lastRaw = false;
    stable = false;
    lastChangeMs = millis();
}

bool ServoMotor360Endstops::DebouncedInput::readStable() {
    if (pin == 255) return false;

    const bool rawLevel = (digitalRead(pin) == HIGH);
    const bool rawActive = activeLow ? !rawLevel : rawLevel; // true si "pulsado/activo"

    const uint32_t now = millis();

    if (rawActive != lastRaw) {
        lastRaw = rawActive;
        lastChangeMs = now;
    }

    // Si el valor se mantiene estable debounceMs, lo aceptamos
    if ((now - lastChangeMs) >= debounceMs) {
        stable = lastRaw;
    }

    return stable;
}

ServoMotor360Endstops::ServoMotor360Endstops(uint8_t servoPin,
                                             const ServoConfig& servoCfg,
                                             const EndstopsConfig& endCfg)
    : ServoMotor360(servoPin, servoCfg),
      endCfg_(endCfg) {
    openSw_.pin = endCfg_.pinOpen;
    openSw_.activeLow = endCfg_.activeLow;
    openSw_.debounceMs = endCfg_.debounceMs;

    closeSw_.pin = endCfg_.pinClose;
    closeSw_.activeLow = endCfg_.activeLow;
    closeSw_.debounceMs = endCfg_.debounceMs;
}

bool ServoMotor360Endstops::begin() {
    const bool ok = ServoMotor360::begin();
    if (!ok) return false;

    openSw_.begin();
    closeSw_.begin();

    // Estado inicial (si hay finales de carrera, intentamos deducirlo)
    if (openLimitActive())  state_ = OPEN;
    else if (closeLimitActive()) state_ = CLOSED;
    else state_ = UNKNOWN;

    return true;
}

void ServoMotor360Endstops::loop() {
    const bool openHit = openLimitActive();
    const bool closeHit = closeLimitActive();

    // Si estamos abriendo y llegamos a OPEN -> parar
    if (state_ == OPENING && openHit) {
        stopWithState(OPEN);
        return;
    }

    // Si estamos cerrando y llegamos a CLOSED -> parar
    if (state_ == CLOSING && closeHit) {
        stopWithState(CLOSED);
        return;
    }

    // Timeout de seguridad
    if ((state_ == OPENING || state_ == CLOSING) && endCfg_.maxRunMs > 0) {
        const uint32_t now = millis();
        if ((now - moveStartMs_) >= endCfg_.maxRunMs) {
            stopWithState(TIMEOUT);
            return;
        }
    }
}

bool ServoMotor360Endstops::openLimitActive() {
    return openSw_.readStable();
}

bool ServoMotor360Endstops::closeLimitActive() {
    return closeSw_.readStable();
}

void ServoMotor360Endstops::startOpening(int speed) {
    // Si ya está abierto, no hacemos nada
    if (openLimitActive()) {
        stopWithState(OPEN);
        return;
    }

    moveSpeed_ = constrain(speed, 0, 100);
    moveStartMs_ = millis();

    // En tu convenio: FORWARD = abrir/subir
    forward(moveSpeed_);
    state_ = (moveSpeed_ > 0) ? OPENING : STOPPED;
}

void ServoMotor360Endstops::startClosing(int speed) {
    // Si ya está cerrado, no hacemos nada
    if (closeLimitActive()) {
        stopWithState(CLOSED);
        return;
    }

    moveSpeed_ = constrain(speed, 0, 100);
    moveStartMs_ = millis();

    // En tu convenio: BACKWARD = cerrar/bajar
    backward(moveSpeed_);
    state_ = (moveSpeed_ > 0) ? CLOSING : STOPPED;
}

void ServoMotor360Endstops::stopWithState(State s) {
    stop();
    state_ = s;
}

String ServoMotor360Endstops::stateString() const {
    switch (state_) {
        case OPEN:     return "OPEN";
        case CLOSED:   return "CLOSED";
        case OPENING:  return "OPENING";
        case CLOSING:  return "CLOSING";
        case STOPPED:  return "STOP";
        case TIMEOUT:  return "TIMEOUT";
        default:       return "UNKNOWN";
    }
}

bool ServoMotor360Endstops::parseCommandEx(const char* in, String& cmdOut, int& speedOut) {
    // Acepta "CMD", "CMD:NN", "CMD NN"
    if (!in || in[0] == '\0') return false;

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

bool ServoMotor360Endstops::applyCommand(const char* command) {
    String cmd;
    int speed = 100;

    if (!parseCommandEx(command, cmd, speed)) return false;
    cmd.toUpperCase();

    if (cmd == "OPEN" || cmd == "UP" || cmd == "FORWARD") {
        startOpening(speed);
        return true;
    }

    if (cmd == "CLOSE" || cmd == "DOWN" || cmd == "BACKWARD") {
        startClosing(speed);
        return true;
    }

    if (cmd == "HOME") {
        // HOME = ir a cerrado hasta fin de carrera close
        startClosing(speed);
        return true;
    }

    if (cmd == "STOP") {
        stopWithState(STOPPED);
        return true;
    }

    // Si quieres mantener compatibilidad con ServoMotor360:
    // return ServoMotor360::applyCommand(command);

    return false;
}
