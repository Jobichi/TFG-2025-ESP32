#include <Devices/Actuators/Relay.h>
#include <Arduino.h>

Relay::Relay(
    uint8_t pin,
    const RelayConfig &cfg
)
    : pin_(pin),
      activeLow_(cfg.activeLow),
      friendlyName_(cfg.friendlyName),
      location_(cfg.location),
      active_(false)
{}

// Inicio del relé
bool Relay::begin() {
    pinMode(pin_, OUTPUT);

    // Estado inicial: relay apagado
    if (activeLow_) {
        digitalWrite(pin_, HIGH);   // HIGH = OFF en relés active-low
    } else {
        digitalWrite(pin_, LOW);    // LOW = OFF en relés active-high
    }

    active_ = false;
    return true;
}

void Relay::loop() {
    // Nada que hacer en bucles, salvo que quieras añadir protecciones
}

// Maneja comandos MQTT SET
bool Relay::applyCommand(const char* command) {

    if (strcmp(command, "ON") == 0) {
        active_ = true;

        if (activeLow_)
            digitalWrite(pin_, LOW);   // encendido en active-low
        else
            digitalWrite(pin_, HIGH);  // encendido en active-high

        return true;
    }

    if (strcmp(command, "OFF") == 0) {
        active_ = false;

        if (activeLow_)
            digitalWrite(pin_, HIGH);  // apagado en active-low
        else
            digitalWrite(pin_, LOW);   // apagado en active-high

        return true;
    }

    // Comando desconocido
    return false;
}
