#include "xpm_font.h"
#include "graphics.h"

int draw_xpm_title_letter(char c, uint16_t x, uint16_t y) {
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

int draw_xpm_sentence_letter(char c, uint16_t x, uint16_t y) {
  switch (c) {
    case 'a': return xpm_image_to_screen((xpm_map_t)font_a, x, y);
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

     // Uppercase
    case 'A': return xpm_image_to_screen((xpm_map_t)font_A, x, y);
    case 'B': return xpm_image_to_screen((xpm_map_t)font_B, x, y);
    case 'C': return xpm_image_to_screen((xpm_map_t)font_C, x, y);
    case 'D': return xpm_image_to_screen((xpm_map_t)font_D, x, y);
    case 'E': return xpm_image_to_screen((xpm_map_t)font_E, x, y);
    case 'F': return xpm_image_to_screen((xpm_map_t)font_F, x, y);
    case 'G': return xpm_image_to_screen((xpm_map_t)font_G, x, y);
    case 'H': return xpm_image_to_screen((xpm_map_t)font_H, x, y);
    case 'I': return xpm_image_to_screen((xpm_map_t)font_I, x, y);
    case 'J': return xpm_image_to_screen((xpm_map_t)font_J, x, y);
    case 'K': return xpm_image_to_screen((xpm_map_t)font_K, x, y);
    case 'L': return xpm_image_to_screen((xpm_map_t)font_L, x, y);
    case 'M': return xpm_image_to_screen((xpm_map_t)font_M, x, y);
    case 'N': return xpm_image_to_screen((xpm_map_t)font_N, x, y);
    case 'O': return xpm_image_to_screen((xpm_map_t)font_O, x, y);
    case 'P': return xpm_image_to_screen((xpm_map_t)font_P, x, y);
    case 'Q': return xpm_image_to_screen((xpm_map_t)font_Q, x, y);
    case 'R': return xpm_image_to_screen((xpm_map_t)font_R, x, y);
    case 'S': return xpm_image_to_screen((xpm_map_t)font_S, x, y);
    case 'T': return xpm_image_to_screen((xpm_map_t)font_T, x, y);
    case 'U': return xpm_image_to_screen((xpm_map_t)font_U, x, y);
    case 'V': return xpm_image_to_screen((xpm_map_t)font_V, x, y);
    case 'W': return xpm_image_to_screen((xpm_map_t)font_W, x, y);
    case 'X': return xpm_image_to_screen((xpm_map_t)font_X, x, y);
    case 'Y': return xpm_image_to_screen((xpm_map_t)font_Y, x, y);
    case 'Z': return xpm_image_to_screen((xpm_map_t)font_Z, x, y);
    default: return 1;
  }
}

int draw_xpm_sentence_letter_red(char c, uint16_t x, uint16_t y) {
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

     // Uppercase
    case 'A': return xpm_image_to_screen((xpm_map_t)font_Ar, x, y);
    case 'B': return xpm_image_to_screen((xpm_map_t)font_Br, x, y);
    case 'C': return xpm_image_to_screen((xpm_map_t)font_Cr, x, y);
    case 'D': return xpm_image_to_screen((xpm_map_t)font_Dr, x, y);
    case 'E': return xpm_image_to_screen((xpm_map_t)font_Er, x, y);
    case 'F': return xpm_image_to_screen((xpm_map_t)font_Fr, x, y);
    case 'G': return xpm_image_to_screen((xpm_map_t)font_Gr, x, y);
    case 'H': return xpm_image_to_screen((xpm_map_t)font_Hr, x, y);
    case 'I': return xpm_image_to_screen((xpm_map_t)font_Ir, x, y);
    case 'J': return xpm_image_to_screen((xpm_map_t)font_Jr, x, y);
    case 'K': return xpm_image_to_screen((xpm_map_t)font_Kr, x, y);
    case 'L': return xpm_image_to_screen((xpm_map_t)font_Lr, x, y);
    case 'M': return xpm_image_to_screen((xpm_map_t)font_Mr, x, y);
    case 'N': return xpm_image_to_screen((xpm_map_t)font_Nr, x, y);
    case 'O': return xpm_image_to_screen((xpm_map_t)font_Or, x, y);
    case 'P': return xpm_image_to_screen((xpm_map_t)font_Pr, x, y);
    case 'Q': return xpm_image_to_screen((xpm_map_t)font_Qr, x, y);
    case 'R': return xpm_image_to_screen((xpm_map_t)font_Rr, x, y);
    case 'S': return xpm_image_to_screen((xpm_map_t)font_Sr, x, y);
    case 'T': return xpm_image_to_screen((xpm_map_t)font_Tr, x, y);
    case 'U': return xpm_image_to_screen((xpm_map_t)font_Ur, x, y);
    case 'V': return xpm_image_to_screen((xpm_map_t)font_Vr, x, y);
    case 'W': return xpm_image_to_screen((xpm_map_t)font_Wr, x, y);
    case 'X': return xpm_image_to_screen((xpm_map_t)font_Xr, x, y);
    case 'Y': return xpm_image_to_screen((xpm_map_t)font_Yr, x, y);
    case 'Z': return xpm_image_to_screen((xpm_map_t)font_Zr, x, y);
    default: return 1;
  }
}

int draw_xpm_sentence_letter_green(char c, uint16_t x, uint16_t y) {
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

     // Uppercase
    case 'A': return xpm_image_to_screen((xpm_map_t)font_Ag, x, y);
    case 'B': return xpm_image_to_screen((xpm_map_t)font_Bg, x, y);
    case 'C': return xpm_image_to_screen((xpm_map_t)font_Cg, x, y);
    case 'D': return xpm_image_to_screen((xpm_map_t)font_Dg, x, y);
    case 'E': return xpm_image_to_screen((xpm_map_t)font_Eg, x, y);
    case 'F': return xpm_image_to_screen((xpm_map_t)font_Fg, x, y);
    case 'G': return xpm_image_to_screen((xpm_map_t)font_Gg, x, y);
    case 'H': return xpm_image_to_screen((xpm_map_t)font_Hg, x, y);
    case 'I': return xpm_image_to_screen((xpm_map_t)font_Ig, x, y);
    case 'J': return xpm_image_to_screen((xpm_map_t)font_Jg, x, y);
    case 'K': return xpm_image_to_screen((xpm_map_t)font_Kg, x, y);
    case 'L': return xpm_image_to_screen((xpm_map_t)font_Lg, x, y);
    case 'M': return xpm_image_to_screen((xpm_map_t)font_Mg, x, y);
    case 'N': return xpm_image_to_screen((xpm_map_t)font_Ng, x, y);
    case 'O': return xpm_image_to_screen((xpm_map_t)font_Og, x, y);
    case 'P': return xpm_image_to_screen((xpm_map_t)font_Pg, x, y);
    case 'Q': return xpm_image_to_screen((xpm_map_t)font_Qg, x, y);
    case 'R': return xpm_image_to_screen((xpm_map_t)font_Rg, x, y);
    case 'S': return xpm_image_to_screen((xpm_map_t)font_Sg, x, y);
    case 'T': return xpm_image_to_screen((xpm_map_t)font_Tg, x, y);
    case 'U': return xpm_image_to_screen((xpm_map_t)font_Ug, x, y);
    case 'V': return xpm_image_to_screen((xpm_map_t)font_Vg, x, y);
    case 'W': return xpm_image_to_screen((xpm_map_t)font_Wg, x, y);
    case 'X': return xpm_image_to_screen((xpm_map_t)font_Xg, x, y);
    case 'Y': return xpm_image_to_screen((xpm_map_t)font_Yg, x, y);
    case 'Z': return xpm_image_to_screen((xpm_map_t)font_Zg, x, y);
    default: return 1;
  }
}

// nao sei se isto vai funcionar talvez seja preciso criar uma funcao draw_xpm_cursor
int draw_xpm_cursor(uint16_t x, uint16_t y) {
  // Use the regular_cursor XPM
  printf("Drawing cursor at: X=%d, Y=%d\n", x, y);
  return xpm_image_to_screen((xpm_map_t)regular_cursor, x, y);
}

