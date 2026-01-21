// ======================= ServoMotor360Endstops.h =======================
#pragma once

#include <Arduino.h>
#include <Devices/Actuators/ServoMotor360.h>

struct EndstopsConfig {
    // Pines de finales de carrera (255 = no usado)
    uint8_t pinOpen  = 255;   // Fin de carrera "abierto"
    uint8_t pinClose = 255;   // Fin de carrera "cerrado"

    // Con INPUT_PULLUP típico: pulsado = LOW
    bool activeLow = true;

    // Antirrebote simple por estabilidad temporal
    uint32_t debounceMs = 30;

    // Seguridad: si no llega al final de carrera, parar
    uint32_t maxRunMs = 15000;
};

class ServoMotor360Endstops : public ServoMotor360 {
public:
    ServoMotor360Endstops(uint8_t servoPin,
                          const ServoConfig& servoCfg = {},
                          const EndstopsConfig& endCfg = {});

    bool begin() override;
    void loop() override;

    // Comandos extendidos: OPEN/CLOSE/STOP + opcional ":NN" o " NN"
    // Incluye "HOME" (cierra hasta fin de carrera close)
    bool applyCommand(const char* command) override;

    String stateString() const override;

    bool isOpen() const { return state_ == OPEN; }
    bool isClosed() const { return state_ == CLOSED; }

private:
    enum State { UNKNOWN, OPEN, CLOSED, OPENING, CLOSING, STOPPED, TIMEOUT };
    State state_{UNKNOWN};

    EndstopsConfig endCfg_;

    // Debounce por pin
    struct DebouncedInput {
        uint8_t pin{255};
        bool activeLow{true};
        uint32_t debounceMs{30};

        bool stable{false};         // último valor estable (true = activo/pulsado)
        bool lastRaw{false};
        uint32_t lastChangeMs{0};

        void begin();
        bool readStable();          // actualiza y devuelve estable (true = activo/pulsado)
    };

    DebouncedInput openSw_;
    DebouncedInput closeSw_;

    // Movimiento activo (para timeout)
    uint32_t moveStartMs_{0};
    int moveSpeed_{100};

private:
    bool openLimitActive();
    bool closeLimitActive();

    void startOpening(int speed);
    void startClosing(int speed);
    void stopWithState(State s);

    static bool parseCommandEx(const char* in, String& cmdOut, int& speedOut);
};