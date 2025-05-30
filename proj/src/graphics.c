#include "graphics.h"
#include "xpm_font.h"

vbe_mode_info_t cur_mode_info;
uint8_t *main_frame_buffer;
uint8_t *secondary_frame_buffer;
uint32_t frame_buffer_size;

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
    
    frame_buffer_size = cur_mode_info.XResolution * cur_mode_info.YResolution * bpp;

    struct minix_mem_range phy_addr;
    phy_addr.mr_base = cur_mode_info.PhysBasePtr;
    phy_addr.mr_limit = phy_addr.mr_base + frame_buffer_size;

    if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &phy_addr)!=0){
        return 1;
    }

    main_frame_buffer = vm_map_phys(SELF, (void*) phy_addr.mr_base, frame_buffer_size);
    if (main_frame_buffer == NULL) {
        printf("Framebuffer mapping failed\n");
        return 1;
    }

    secondary_frame_buffer = malloc(frame_buffer_size);
    if (secondary_frame_buffer == NULL) {
        printf("Failed to allocate secondary buffer\n");
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

    if (secondary_frame_buffer == NULL) return 1;

    if (x > cur_mode_info.XResolution || y > cur_mode_info.YResolution){
        //we don't check if is smaller than 0 'cause is an unsigned number
        return 1;
    }

    unsigned int bpp = (cur_mode_info.BitsPerPixel + 7) / 8;
    unsigned int pixel_index = (cur_mode_info.XResolution * y + x) * bpp;
    if (pixel_index >= frame_buffer_size) return 1;

    uint32_t norm_color;
    normalization_color(color, &norm_color);

    if (memcpy(&secondary_frame_buffer[pixel_index], &norm_color, bpp) == NULL) {
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

int draw_xpm_title(const char* str, uint16_t x, uint16_t y) {
    int spacing = 36;
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (str[i] != ' ') {
            draw_xpm_title_letter(str[i], x + i * spacing, y);
        }
    }
    return 0;
}

int draw_xpm_sentence(const char* str, uint16_t x, uint16_t y, const char* color) {
    int spacing = 15;
    for (size_t i = 0; str[i] != '\0'; i++) {
        printf("Drawing char: %c at %d,%d\n", str[i], x + i * spacing, y);
        if ((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z') || str[i] != ' ') {
            if (strcmp(color, "green") == 0) {
                draw_xpm_sentence_letter_green(str[i], x + i * spacing, y);
            }
            else if (strcmp(color, "red") == 0) {
                draw_xpm_sentence_letter_red(str[i], x + i * spacing, y);
            }
            else {
                draw_xpm_sentence_letter(str[i], x + i * spacing, y);
            }
        }
    }
    return 0;
}

void swap_buffers() {
    memcpy(main_frame_buffer, secondary_frame_buffer, frame_buffer_size);
}
