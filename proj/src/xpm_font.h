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

//buttons
#include "xpm_fonts/sec_15.xpm"
#include "xpm_fonts/sec_30.xpm"
#include "xpm_fonts/sec_60.xpm"

//language buttons
#include "xpm_fonts/eng.xpm"
#include "xpm_fonts/por.xpm"
#include "xpm_fonts/esp.xpm"


int (draw_xpm_title_letter)(char c, uint16_t x, uint16_t y);
int (draw_xpm_sentence_letter)(char c, uint16_t x, uint16_t y);
int (draw_xpm_sentence_letter_red)(char c, uint16_t x, uint16_t y);
int (draw_xpm_sentence_letter_green)(char c, uint16_t x, uint16_t y);

int (draw_xpm_cursor)(uint16_t x, uint16_t y);
int (draw_button_15)(uint16_t x, uint16_t y);
int (draw_button_30)(uint16_t x, uint16_t y);
int (draw_button_60)(uint16_t x, uint16_t y);

int (draw_button_eng)(uint16_t x, uint16_t y);
int (draw_button_por)(uint16_t x, uint16_t y);
int (draw_button_esp)(uint16_t x, uint16_t y);


#endif // XPM_FONT_H




