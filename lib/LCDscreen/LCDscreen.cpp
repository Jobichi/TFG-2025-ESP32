#include <LCDscreen.h>

// Constructor para definir los parámetros de la pantalla.
LCDScreen::LCDScreen(uint8_t address, int cols, int rows)
    : _lcd(address, cols, rows), _cols(cols), _rows(rows) {}

// Método para inicializar pantalla.
void LCDScreen::begin() {
    _lcd.init();
    _lcd.backlight();
    _lcd.clear();
}

// Método para escribir una línea.
void LCDScreen::printLine(int line, const String &message) {
    String padded = message;
    int padding = _cols - padded.length();

    if (padding > 0) {
        for (int i = 0; i < padding; ++i) {
            padded += ' ';
        }
    } else if (padding < 0) {
        padded = padded.substring(0, _cols);
    }

    _lcd.setCursor(0, line);
    _lcd.print(padded);
}


// Método para centrar el mensaje.
void LCDScreen::printCentered(int line, const String &message) {
    printLine(line, ""); // Limpia línea
    int padding = (_cols - message.length()) / 2;
    _lcd.setCursor(padding > 0 ? padding : 0, line);
    _lcd.print(message);
}

// Método para iniciar el scrolling de un mansaje.
void LCDScreen::startScrolling(int line, const String &message, int delayMs){
    // Comprobamos primero si el mensaje entra en la misma línea sin scroll:
    if(message.length() <= _cols){
        printLine(line, message);
        _scrollingActive = false;
        return;
    }

    String padding(_cols, ' ');
    _scrollMessage = padding + message + padding;
    _scrollLine = line;
    _scrollDelay = delayMs;
    _scrollIndex = 0;
    _scrollingActive = true;
    _lastScrollTime = millis();
    _scrollStartTime = _lastScrollTime;

    // Calculamos el tiempo de duración minimo para el scroll:
    _minScrollDuration = _scrollMessage.length() * delayMs;
}

// Método para actualizar el scroll del mensaje.
void LCDScreen::updateScrolling() {
    if (!_scrollingActive) 
        return;

    unsigned long now = millis();

    if (now - _lastScrollTime >= _scrollDelay) {
        // Comprobamos si ya hemos mostrado todo el mensaje + espacio vacío
        if (_scrollIndex + _cols > _scrollMessage.length()) {
            _scrollingActive = false;
            _lcd.setCursor(0, _scrollLine);
            _lcd.print(String(" ", _cols));  // Limpia la línea tras terminar
            return;
        }

        // Mostrar la subcadena desplazada
        _lcd.setCursor(0, _scrollLine);
        _lcd.print(_scrollMessage.substring(_scrollIndex, _scrollIndex + _cols));
        _scrollIndex++;
        _lastScrollTime = now;
    }
}

// Método para parar el scroll del mensaje.
void LCDScreen::stopScrolling(){
    // Comprobamos de que no esté parado con anterioridad:
    if(!_scrollingActive)
        return;

    _scrollingActive = false;
    _lcd.setCursor(0, _scrollLine);
    _lcd.print(String(" ", _cols));
}

// Método para limpiar la pantalla LCD.
void LCDScreen::clear() {
    _lcd.clear();
}


