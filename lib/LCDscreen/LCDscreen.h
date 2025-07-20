#pragma once

#include <LiquidCrystal_I2C.h>

/*
    Clase encargada de manejar la pantalla LCD (vía I2C),
    mostrando mensajes personalizados del sistema.
        message -> Mensaje a ser mostrado.
        line -> Cual de las líneas del LCD.
        delay -> Tiempo de actualización en el scroll.
*/

class LCDScreen {
    private:
        LiquidCrystal_I2C _lcd;
        int _cols;
        int _rows;

        // Parametros para el scroll del mensaje.
        bool _scrollingActive = false;
        unsigned long _lastScrollTime = 0;
        unsigned long _scrollStartTime = 0;
        unsigned long _minScrollDuration = 0;
        int _scrollIndex = 0;
        String _scrollMessage;
        int _scrollLine = 0;  // Primera línea por defecto para los mensajes
        int _scrollDelay = 300;

    public:
        // Constructor - Inicializado por defecto para nuestra pantalla.
        LCDScreen(uint8_t address = 0x27, int cols = 16, int rows = 2);

        // Inicializa el LCD
        void begin();

        // Mostrar un mensaje en la línea deseada
        void printLine(int line, const String &message);

        // Mostramos un mensaje en un scroll cíclico y métodos para cambiarlos
        void startScrolling(int line, const String &message, int delay = 300);
        void stopScrolling();
        void updateScrolling();
        bool isScrolling() const { return _scrollingActive; }

        // Limpia la pantalla
        void clear();

        // Mostrar mensaje centrado
        void printCentered(int line, const String &message);
};
