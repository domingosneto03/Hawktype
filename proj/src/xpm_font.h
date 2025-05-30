#ifndef XPM_FONT_H
#define XPM_FONT_H

#include <minix/sysutil.h>
#include "i8042.h"
#include <lcom/lcf.h>

// Title font (Striger)
#include "xpm_fonts/xpm_title/hawktype.xpm"

// Sentence font (Cute Pixel)

//lower
#include "xpm_fonts/xpm_lower/default.xpm"
#include "xpm_fonts/xpm_lower/green.xpm"
#include "xpm_fonts/xpm_lower/red.xpm"

//upper
#include "xpm_fonts/xpm_upper/default.xpm"
#include "xpm_fonts/xpm_upper/green.xpm"
#include "xpm_fonts/xpm_upper/red.xpm"

//mouse cursor
#include "xpm_fonts/regular_cursor.xpm"

int (draw_xpm_title_letter)(char c, uint16_t x, uint16_t y);
int (draw_xpm_sentence_letter)(char c, uint16_t x, uint16_t y);
int (draw_xpm_sentence_letter_red)(char c, uint16_t x, uint16_t y);
int (draw_xpm_sentence_letter_green)(char c, uint16_t x, uint16_t y);

int (draw_xpm_cursor)(uint16_t x, uint16_t y);

#endif // XPM_FONT_H




