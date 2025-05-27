#include "graphics.h"
#include "font.h"
#include "title_xpm.h"

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

int (normalization_color)(uint32_t color, uint32_t *new_color){
    if (cur_mode_info.BitsPerPixel == 32){
        *new_color = color;
    }
    else{
        *new_color = color & (BIT(cur_mode_info.BitsPerPixel)-1);
    }
    return 0;
}

int (draw_pixel)(uint16_t x,uint16_t y,uint32_t color){
    if (x > cur_mode_info.XResolution || y > cur_mode_info.YResolution){
        return 1;
    }

    unsigned int bpp = (cur_mode_info.BitsPerPixel + 7) / 8;
    unsigned int pixel_index = (cur_mode_info.XResolution * y + x)* bpp;
    
    uint32_t norm_color;
    normalization_color(color, &norm_color);

    if (memcpy(&frame_buf[pixel_index], &norm_color, bpp)== NULL){
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

int (xpm_image_to_screen)(xpm_map_t xmp, uint16_t x, uint16_t y){

    printf(" -> entered xpm_image_to_screen on H\n");
    xpm_image_t image;
    uint8_t *colors = xpm_load(xmp, XPM_INDEXED, &image);
    if (!colors) {
        printf(" -> xpm_load failed\n");
        return 1;
    }
    for (int i = 0; i < image.height; i++){
        for (int j = 0; j < image.width; j++){
            if(draw_pixel(x + j, y + i, *colors)!=0){
                return 1;
            }
            colors++;
        }
    }
    printf(" -> xpm reaches here\n");
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
    case 'H': printf(" -> calling xpm_image_to_screen on H\n"); return xpm_image_to_screen(title_H, x, y);
    /*
    case 'A': return xpm_image_to_screen((xpm_map_t)title_A, x, y);
    case 'W': return xpm_image_to_screen((xpm_map_t)title_W, x, y);
    case 'K': return xpm_image_to_screen((xpm_map_t)title_K, x, y);
    case 'T': return xpm_image_to_screen((xpm_map_t)title_T, x, y);
    case 'Y': return xpm_image_to_screen((xpm_map_t)title_Y, x, y);
    case 'P': return xpm_image_to_screen((xpm_map_t)title_P, x, y);
    case 'E': return xpm_image_to_screen((xpm_map_t)title_E, x, y);
    */
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


