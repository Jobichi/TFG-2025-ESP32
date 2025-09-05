#include "Screens/OledManager.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

static const int CHAR_WIDTH = 6;   // ancho de carácter base (5+1) * textSize
static const int LINE_HEIGHT = 8;  // alto de carácter base

// --- Estado del scroll ---
static String topText = "";
static int topSize = 1;
static int scrollX = 0; // posición horizontal actual (para textos largos)

static String bottomText = "";
static int bottomSize = 1;

static unsigned long lastScroll = 0;
static const unsigned long scrollInterval = 50; // velocidad scroll (ms)

// --- API ---

void setupOLED() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("[OLED] No se pudo inicializar la pantalla"));
        for (;;); // bloqueo si falla
    }
    // Evitar que el texto salte de línea automáticamente.
    display.setTextWrap(false);

    display.clearDisplay();
    display.display();
}

void oledClear() {
    display.clearDisplay();
    display.display();
}

void oledSetTopText(const String& text, int textSize) {
    topText = text;
    topSize = textSize;
    scrollX = SCREEN_WIDTH; // arranca fuera de la pantalla (derecha)
}

void oledSetBottomText(const String& text, int textSize) {
    bottomText = text;
    bottomSize = textSize;
}

void oledHandle() {
    unsigned long now = millis();

    // --- Redibujar pantalla ---
    display.clearDisplay();

    // ---------- LÍNEA SUPERIOR ----------
    display.setTextSize(topSize);
    display.setTextColor(SSD1306_WHITE);

    const int textWidthTop = topText.length() * CHAR_WIDTH * topSize;
    const int yTop = 0;

    if (textWidthTop > SCREEN_WIDTH) {
        // Sólo scrollea si el texto NO cabe.
        if (now - lastScroll >= scrollInterval) {
            lastScroll = now;
            scrollX--;  // desplazar hacia la izquierda
            if (scrollX + textWidthTop < 0) {
                scrollX = SCREEN_WIDTH; // reinicia desde la derecha
            }
        }
        display.setCursor(scrollX, yTop);
        display.println(topText);

        // (Opcional: para scroll continuo sin “hueco”, dibuja una segunda copia)
        // int gap = 16; // separación visual
        // int x2 = scrollX + textWidthTop + gap;
        // if (x2 < SCREEN_WIDTH) {
        //     display.setCursor(x2, yTop);
        //     display.println(topText);
        // }
    } else {
        // Si cabe, céntralo y NO scrollees (evita saltos de línea absurdos).
        const int xTop = (SCREEN_WIDTH - textWidthTop) / 2;
        display.setCursor(xTop < 0 ? 0 : xTop, yTop);
        display.println(topText);

        // Prepara el siguiente scroll (si luego cambias a un texto largo).
        scrollX = SCREEN_WIDTH;
    }

    // ---------- LÍNEA INFERIOR ----------
    if (bottomText.length() > 0) {
        display.setTextSize(bottomSize);
        display.setTextColor(SSD1306_WHITE);

        const int textWidthBottom = bottomText.length() * CHAR_WIDTH * bottomSize;

        // Centrar horizontalmente
        int xBottom = (SCREEN_WIDTH - textWidthBottom) / 2;
        if (xBottom < 0) xBottom = 0;

        // Posicionar pegado a la base (sin *2, que lo subía más de la cuenta)
        const int yBottom = SCREEN_HEIGHT - LINE_HEIGHT * bottomSize;

        display.setCursor(xBottom, yBottom);
        display.println(bottomText);
    }

    display.display();
}
