#include "graphics.h"
#include "font.h"
#include "include_xpm.h"

vbe_mode_info_t cur_mode_info;
uint8_t* frame_buf;

int (set_graphic_mode)(uint16_t submode){
    reg86_t reg86;
    memset(&reg86,0,sizeof(reg86));
    reg86.intno = INTNO;
    reg86.ah = 0x4f;
    reg86.al = 0x02;
    reg86.bx = submode | BIT(14);
    if (sys_int86(&reg86)!=0){
        perror("Failed setting up graphics.\n");
        return 1;
    }
    return 0;
}

int (set_frame_buffer)(uint16_t mode){

    memset(&cur_mode_info, 0, sizeof(cur_mode_info));
    if(vbe_get_mode_info(mode, &cur_mode_info)!=0){
        return 1;
    }

    //bytes per pixel
    unsigned int bpp = (cur_mode_info.BitsPerPixel+7)/8;
    unsigned frame_needed_size = cur_mode_info.XResolution * cur_mode_info.YResolution * bpp;

    struct minix_mem_range phy_addr;
    phy_addr.mr_base = cur_mode_info.PhysBasePtr;
    phy_addr.mr_limit = phy_addr.mr_base + frame_needed_size;

    if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &phy_addr)!=0){
        return 1;
    }

    frame_buf = vm_map_phys(SELF, (void*) phy_addr.mr_base, frame_needed_size);
    if (frame_buf ==NULL){
        printf("Framebuffer mapping failed\n");
        return 1;
    }

    return 0;
}

int normalization_color(uint32_t color, uint32_t *new_color) {
    if (cur_mode_info.MemoryModel == DIRECT_COLOR) {
        uint32_t r = (color >> 16) & 0xFF;
        uint32_t g = (color >> 8)  & 0xFF;
        uint32_t b =  color        & 0xFF;

        r = (r >> (8 - cur_mode_info.RedMaskSize))   << cur_mode_info.RedFieldPosition;
        g = (g >> (8 - cur_mode_info.GreenMaskSize)) << cur_mode_info.GreenFieldPosition;
        b = (b >> (8 - cur_mode_info.BlueMaskSize))  << cur_mode_info.BlueFieldPosition;

        *new_color = r | g | b;
    } else {
        *new_color = color;
    }

    return 0;
}

int (draw_pixel)(uint16_t x,uint16_t y,uint32_t color){

    if (frame_buf == NULL) return 1;

    if (x > cur_mode_info.XResolution || y > cur_mode_info.YResolution){
        //we don't check if is smaller than 0 'cause is an unsigned number
        return 1;
    }

    unsigned int bpp = (cur_mode_info.BitsPerPixel + 7) / 8;

    unsigned int pixel_index = (cur_mode_info.XResolution * y + x)* bpp;
    if (pixel_index >= cur_mode_info.XResolution * cur_mode_info.YResolution * bpp) return 1;

    uint32_t norm_color;
    normalization_color(color, &norm_color);

    if (memcpy(&frame_buf[pixel_index], &norm_color, bpp) == NULL) {
        return 1;
    }
    return 0;
}


int (draw_horizontal_line)(uint16_t x, uint16_t y,uint16_t width,uint32_t color){
    for (int i = 0; i < width; i++){
        if (draw_pixel(x+i, y, color)!=0){
            return 1;
        }
    }
    return 0;
}

int (draw_rectangle)(uint16_t x,uint16_t y, uint16_t width, uint16_t height, uint32_t color){
    for (int i = 0; i < height; i++){
        if (draw_horizontal_line(x, y+i, width, color)!=0){
            return 1;
        }
    }
    return 0;
}

int (indexed_mode_color)(uint16_t col, uint16_t row, uint8_t step, 
                         uint32_t first, uint8_t n, uint32_t *color){

    *color = (first + (row * n + col) * step % (1 << cur_mode_info.BitsPerPixel));
    return 0;
}

int (direct_mode)(int j, int i, uint32_t first, uint8_t step,uint32_t *color){
    uint32_t R,G,B;

    R = ((((1 << cur_mode_info.RedMaskSize) - 1) & 
        (first >> cur_mode_info.RedFieldPosition)) + j * step) 
        % (1 << cur_mode_info.RedMaskSize);
    
    G = ((((1 << cur_mode_info.GreenMaskSize) - 1) & 
        (first >> cur_mode_info.GreenFieldPosition)) + i * step) 
        % (1 << cur_mode_info.GreenMaskSize);

    B = ((((1 << cur_mode_info.BlueMaskSize) - 1) & 
        (first >> cur_mode_info.BlueFieldPosition)) + (i + j) * step) 
        % (1 << cur_mode_info.BlueMaskSize);

    *color = (R << cur_mode_info.RedFieldPosition | 
              G << cur_mode_info.GreenFieldPosition |
              B << cur_mode_info.BlueFieldPosition);


    //R(row, col) = (R(first) + col * step) % (1 << RedMaskSize)
    //G(row, col) = (G(first) + row * step) % (1 << GreenMaskSize)
    //B(row, col) = (B(first) + (col + row) * step) % (1 << BlueMaskSize)
    return 0;
}

int xpm_image_to_screen(xpm_map_t xmp, uint16_t x, uint16_t y) {
    xpm_image_t image;
    uint8_t *raw_data = xpm_load(xmp, XPM_8_8_8_8, &image);
    if (!raw_data) {
        printf(" -> xpm_load failed\n");
        return 1;
    }

    uint32_t *pixel_data = (uint32_t *)raw_data;
    for (int i = 0; i < image.height; i++) {
        for (int j = 0; j < image.width; j++) {
            uint32_t color = pixel_data[i * image.width + j];
            draw_pixel(x + j, y + i, color);
        }
    }
    return 0;
}


int draw_char(uint16_t x, uint16_t y, char c, uint32_t color) {
    uint8_t *glyph = font_bitmaps[(uint8_t)c];
    for (int row = 0; row < 16; row++)
        for (int col = 0; col < 8; col++)
            if (glyph[row] & (1 << (7 - col)))
                if (draw_pixel(x + col, y + row, color))
                    return 1;
    return 0;
}

int draw_xpm_title_letter(char c, uint16_t x, uint16_t y) {
     printf(" -> drawing %c\n", c);
  switch (c) {
    case 'H': return xpm_image_to_screen((xpm_map_t)title_H, x, y);
    case 'A': return xpm_image_to_screen((xpm_map_t)title_A, x, y);
    case 'W': return xpm_image_to_screen((xpm_map_t)title_W, x, y);
    case 'K': return xpm_image_to_screen((xpm_map_t)title_K, x, y);
    case 'T': return xpm_image_to_screen((xpm_map_t)title_T, x, y);
    case 'Y': return xpm_image_to_screen((xpm_map_t)title_Y, x, y);
    case 'P': return xpm_image_to_screen((xpm_map_t)title_P, x, y);
    case 'E': return xpm_image_to_screen((xpm_map_t)title_E, x, y);
    default: return 1; // unknown char
  }
}

int draw_xpm_title(const char* str, uint16_t x, uint16_t y) {
    int spacing = 36;
    for (size_t i = 0; str[i] != '\0'; i++) {
        printf("drawing letter: %c\n", str[i]);
        if (str[i] != ' ') {
            int result = draw_xpm_title_letter(str[i], x + i * spacing, y);
            if (result != 0) {
                printf("Failed to draw letter: %c\n", str[i]);
                return 1;
            }
        }
    }
    return 0;
}

int draw_xpm_sentence_letter(char c, uint16_t x, uint16_t y) {
  switch (c) {
    case 'a': return xpm_image_to_screen((xpm_map_t)font_a, x, y); //escreves uma letra em x
    case 'b': return xpm_image_to_screen((xpm_map_t)font_b, x, y);
    case 'c': return xpm_image_to_screen((xpm_map_t)font_c, x, y);
    case 'd': return xpm_image_to_screen((xpm_map_t)font_d, x, y);
    case 'e': return xpm_image_to_screen((xpm_map_t)font_e, x, y);
    case 'f': return xpm_image_to_screen((xpm_map_t)font_f, x, y);
    case 'g': return xpm_image_to_screen((xpm_map_t)font_g, x, y);
    case 'h': return xpm_image_to_screen((xpm_map_t)font_h, x, y);
    case 'i': return xpm_image_to_screen((xpm_map_t)font_i, x, y);
    case 'j': return xpm_image_to_screen((xpm_map_t)font_j, x, y);
    case 'k': return xpm_image_to_screen((xpm_map_t)font_k, x, y);
    case 'l': return xpm_image_to_screen((xpm_map_t)font_l, x, y);
    case 'm': return xpm_image_to_screen((xpm_map_t)font_m, x, y);
    case 'n': return xpm_image_to_screen((xpm_map_t)font_n, x, y);
    case 'o': return xpm_image_to_screen((xpm_map_t)font_o, x, y);
    case 'p': return xpm_image_to_screen((xpm_map_t)font_p, x, y);
    case 'q': return xpm_image_to_screen((xpm_map_t)font_q, x, y);
    case 'r': return xpm_image_to_screen((xpm_map_t)font_r, x, y);
    case 's': return xpm_image_to_screen((xpm_map_t)font_s, x, y);
    case 't': return xpm_image_to_screen((xpm_map_t)font_t, x, y);
    case 'u': return xpm_image_to_screen((xpm_map_t)font_u, x, y);
    case 'v': return xpm_image_to_screen((xpm_map_t)font_v, x, y);
    case 'w': return xpm_image_to_screen((xpm_map_t)font_w, x, y);
    case 'x': return xpm_image_to_screen((xpm_map_t)font_x, x, y);
    case 'y': return xpm_image_to_screen((xpm_map_t)font_y, x, y);
    case 'z': return xpm_image_to_screen((xpm_map_t)font_z, x, y);
    default: return 1;
  }
}

int draw_xpm_sentence_letterRed(char c, uint16_t x, uint16_t y) {
  switch (c) {
    case 'a': return xpm_image_to_screen((xpm_map_t)font_ar, x, y);
    case 'b': return xpm_image_to_screen((xpm_map_t)font_br, x, y);
    case 'c': return xpm_image_to_screen((xpm_map_t)font_cr, x, y);
    case 'd': return xpm_image_to_screen((xpm_map_t)font_dr, x, y);
    case 'e': return xpm_image_to_screen((xpm_map_t)font_er, x, y);
    case 'f': return xpm_image_to_screen((xpm_map_t)font_fr, x, y);
    case 'g': return xpm_image_to_screen((xpm_map_t)font_gr, x, y);
    case 'h': return xpm_image_to_screen((xpm_map_t)font_hr, x, y);
    case 'i': return xpm_image_to_screen((xpm_map_t)font_ir, x, y);
    case 'j': return xpm_image_to_screen((xpm_map_t)font_jr, x, y);
    case 'k': return xpm_image_to_screen((xpm_map_t)font_kr, x, y);
    case 'l': return xpm_image_to_screen((xpm_map_t)font_lr, x, y);
    case 'm': return xpm_image_to_screen((xpm_map_t)font_mr, x, y);
    case 'n': return xpm_image_to_screen((xpm_map_t)font_nr, x, y);
    case 'o': return xpm_image_to_screen((xpm_map_t)font_or, x, y);
    case 'p': return xpm_image_to_screen((xpm_map_t)font_pr, x, y);
    case 'q': return xpm_image_to_screen((xpm_map_t)font_qr, x, y);
    case 'r': return xpm_image_to_screen((xpm_map_t)font_rr, x, y);
    case 's': return xpm_image_to_screen((xpm_map_t)font_sr, x, y);
    case 't': return xpm_image_to_screen((xpm_map_t)font_tr, x, y);
    case 'u': return xpm_image_to_screen((xpm_map_t)font_ur, x, y);
    case 'v': return xpm_image_to_screen((xpm_map_t)font_vr, x, y);
    case 'w': return xpm_image_to_screen((xpm_map_t)font_wr, x, y);
    case 'x': return xpm_image_to_screen((xpm_map_t)font_xr, x, y);
    case 'y': return xpm_image_to_screen((xpm_map_t)font_yr, x, y);
    case 'z': return xpm_image_to_screen((xpm_map_t)font_zr, x, y);
    default: return 1;
  }
}

int draw_xpm_sentence_letterGreen(char c, uint16_t x, uint16_t y) {
  switch (c) {
    case 'a': return xpm_image_to_screen((xpm_map_t)font_ag, x, y);
    case 'b': return xpm_image_to_screen((xpm_map_t)font_bg, x, y);
    case 'c': return xpm_image_to_screen((xpm_map_t)font_cg, x, y);
    case 'd': return xpm_image_to_screen((xpm_map_t)font_dg, x, y);
    case 'e': return xpm_image_to_screen((xpm_map_t)font_eg, x, y);
    case 'f': return xpm_image_to_screen((xpm_map_t)font_fg, x, y);
    case 'g': return xpm_image_to_screen((xpm_map_t)font_gg, x, y);
    case 'h': return xpm_image_to_screen((xpm_map_t)font_hg, x, y);
    case 'i': return xpm_image_to_screen((xpm_map_t)font_ig, x, y);
    case 'j': return xpm_image_to_screen((xpm_map_t)font_jg, x, y);
    case 'k': return xpm_image_to_screen((xpm_map_t)font_kg, x, y);
    case 'l': return xpm_image_to_screen((xpm_map_t)font_lg, x, y);
    case 'm': return xpm_image_to_screen((xpm_map_t)font_mg, x, y);
    case 'n': return xpm_image_to_screen((xpm_map_t)font_ng, x, y);
    case 'o': return xpm_image_to_screen((xpm_map_t)font_og, x, y);
    case 'p': return xpm_image_to_screen((xpm_map_t)font_pg, x, y);
    case 'q': return xpm_image_to_screen((xpm_map_t)font_qg, x, y);
    case 'r': return xpm_image_to_screen((xpm_map_t)font_rg, x, y);
    case 's': return xpm_image_to_screen((xpm_map_t)font_sg, x, y);
    case 't': return xpm_image_to_screen((xpm_map_t)font_tg, x, y);
    case 'u': return xpm_image_to_screen((xpm_map_t)font_ug, x, y);
    case 'v': return xpm_image_to_screen((xpm_map_t)font_vg, x, y);
    case 'w': return xpm_image_to_screen((xpm_map_t)font_wg, x, y);
    case 'x': return xpm_image_to_screen((xpm_map_t)font_xg, x, y);
    case 'y': return xpm_image_to_screen((xpm_map_t)font_yg, x, y);
    case 'z': return xpm_image_to_screen((xpm_map_t)font_zg, x, y);
    default: return 1;
  }
}

int draw_xpm_sentence(const char* str, uint16_t x, uint16_t y) {
  int spacing = 15;
  for (size_t i = 0; str[i] != '\0'; i++) {
    if (str[i] != ' ') {
      draw_xpm_sentence_letter(str[i], x + i * spacing, y);
    }
  }
  return 0;
}

int draw_xpm_sentence_red(const char* str, uint16_t x, uint16_t y) {
  int spacing = 15;
  for (size_t i = 0; str[i] != '\0'; i++) {
    if (str[i] != ' ') {
      draw_xpm_sentence_letterRed(str[i], x + i * spacing, y);
    }
  }
  return 0;
}

int draw_xpm_sentence_green(const char* str, uint16_t x, uint16_t y) {
  int spacing = 15;
  for (size_t i = 0; str[i] != '\0'; i++) {
    if (str[i] != ' ') {
      draw_xpm_sentence_letterGreen(str[i], x + i * spacing, y);
    }
  }
  return 0;
}
