#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <minix/sysutil.h>
#include "i8042.h"
#include <lcom/lcf.h>

int (set_graphic_mode)(uint16_t submode);

int (set_frame_buffer)(uint16_t mode);

int (normalization_color)(uint32_t color, uint32_t *new_color);

int (draw_rectangle)(uint16_t x,uint16_t y, uint16_t width, uint16_t height, uint32_t color);

int (draw_horizontal_line)(uint16_t x, uint16_t y,uint16_t width,uint32_t color);

int (draw_pixel)(uint16_t x,uint16_t y,uint32_t color);

int (indexed_mode_color)(uint16_t col, uint16_t row, uint8_t step, uint32_t first, uint8_t n, uint32_t *color);

int (direct_mode)(int j, int i, uint32_t first, uint8_t step, uint32_t *color);

int (xpm_image_to_screen)(xpm_map_t xmp, uint16_t x, uint16_t y);

int (draw_char)(uint16_t x, uint16_t y, char c, uint32_t color);

#endif
