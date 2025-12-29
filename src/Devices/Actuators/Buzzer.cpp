#include <Devices/Actuators/Buzzer.h>
#include <Arduino.h>

Buzzer::Buzzer(
    uint8_t pin,
    const char* friendlyName,
    const char* location
) :
    pin_(pin),
    friendlyName_(friendlyName),
    location_(location),
    active_(false)
{}

// Configuración inicial del pin
bool Buzzer::begin() {
    pinMode(pin_, OUTPUT);
    digitalWrite(pin_, LOW);
    active_ = false;
    return true;
}

// Si en un futuro necesitas beeps temporizados, se coloca aquí
void Buzzer::loop() {
    // Actualmente sin lógica periódica
}

// Procesa comandos desde MQTT SET
bool Buzzer::applyCommand(const char* command) {

    if (strcmp(command, "ON") == 0) {
        active_ = true;
        digitalWrite(pin_, HIGH);
        return true;
    }

    if (strcmp(command, "OFF") == 0) {
        active_ = false;
        digitalWrite(pin_, LOW);
        return true;
    }

    // Modo beep temporal
    if (strcmp(command, "BEEP") == 0) {
        active_ = true;
        tone(pin_, 1000, 300); // beep 1kHz durante 300ms
        // No mantiene activo el actuador (estado vuelve a OFF)
        active_ = false;
        return true;
    }

    // Comando desconocido
    return false;
}
