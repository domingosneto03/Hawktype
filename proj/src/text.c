#include "graphics.h"
#include "text.h"
#include "font.h"

// Draws text as simple boxes for now
int draw_text(const char* str, uint16_t x, uint16_t y, uint32_t color) {
    int spacing = 10;
     for (size_t i = 0; str[i] != '\0'; i++) {
        char c = str[i];

        if (c != ' ') {
            if (draw_char(x + i * spacing, y, c, color) != 0) {
                return 1;
            }
        }
    }
    return 0;
}
