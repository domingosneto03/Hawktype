#include "graphics.h"
#include "font.h"

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

    if (frame_buf == NULL) return 1;

    if (x > cur_mode_info.XResolution || y > cur_mode_info.YResolution){
        //we don't check if is smaller than 0 'cause is an unsigned number
        return 1;
    }

    unsigned int bpp = (cur_mode_info.BitsPerPixel + 7) / 8;

    unsigned int pixel_index = (cur_mode_info.XResolution * y + x)* bpp;
    if (pixel_index >= cur_mode_info.XResolution * cur_mode_info.YResolution * bpp) return 1;

    if (memcpy(&frame_buf[pixel_index], &color, bpp)== NULL){
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

    xpm_image_t image;

    uint8_t *colors = xpm_load(xmp, XPM_INDEXED, &image);

    for (int i = 0; i < image.height; i++){
        for (int j = 0; j < image.width; j++){
            if(draw_pixel(x + j, y + i, *colors)!=0){
                return 1;
            }
            colors++;
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
