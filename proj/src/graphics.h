#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "xpm_font.h"

// Graphics mode and buffer setup
int set_graphic_mode(uint16_t submode);
int set_frame_buffer(uint16_t mode);

// Drawing primitives
int draw_pixel(uint16_t x, uint16_t y, uint32_t color);
int draw_horizontal_line(uint16_t x, uint16_t y, uint16_t width, uint32_t color);
int draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

// VBE color calculations
int normalization_color(uint32_t color, uint32_t *new_color);
int indexed_mode_color(uint16_t col, uint16_t row, uint8_t step, uint32_t first, uint8_t n, uint32_t *color);
int direct_mode(int j, int i, uint32_t first, uint8_t step, uint32_t *color);

// XPM rendering
int xpm_image_to_screen(xpm_map_t xmp, uint16_t x, uint16_t y);
int draw_xpm_title(const char* str, uint16_t x, uint16_t y);
int draw_xpm_sentence(const char* str, uint16_t x, uint16_t y, const char* color);

// Optimized update functions
int draw_static_background();

// Double buffering
void swap_buffers();

// Extern framebuffer and mode info
extern vbe_mode_info_t cur_mode_info;
extern uint8_t *main_frame_buffer;
extern uint8_t *secondary_frame_buffer;
extern uint32_t frame_buffer_size;

#endif // GRAPHICS_H
