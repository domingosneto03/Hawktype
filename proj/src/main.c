#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "i8042.h"
#include "keyboard.h"
#include "graphics.h"
#include "timer.c"
#include "i8254.h"
#include "words_list.h"

extern vbe_mode_info_t cur_mode_info;
extern uint8_t cur_scancode;
extern int timer_counter; 

char cur_typed_word[MAX_WORD_SIZE] = "";

enum gamestate { WAITING, STARTED, STATS, RESETING, EXIT };
enum wordstate { NOTCHECKED, CORRECT, WRONG };
enum game_languages { ENG, PT, ESP };
enum game_languages cur_game_lang = ENG;

struct words {
    char word[MAX_WORD_SIZE]; 
    enum wordstate state;
};

struct words word_list[MAX_GAME_WORDS];

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

    // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int draw_words() {
    int margin_x = 60, line_spacing = 30, word_spacing = 15;
    int cursor_x = margin_x, cursor_y = 200;
    int max_x = cur_mode_info.XResolution - margin_x;
    for (int i = 0; i < 25; i++) {
        int word_width = strlen(word_list[i].word) * 15;
        if (cursor_x + word_width > max_x) {
            cursor_x = margin_x;
            cursor_y += line_spacing;
        }
        draw_xpm_sentence(word_list[i].word, cursor_x, cursor_y, "default");
        switch (word_list[i].state) {
            case CORRECT: draw_xpm_sentence(word_list[i].word, cursor_x, cursor_y, "green"); break;
            case WRONG: draw_xpm_sentence(word_list[i].word, cursor_x, cursor_y, "red"); break;
            default: break;
        }
        cursor_x += word_width + word_spacing;
    }
    return 0;
}

int draw_textbox_content() {
    int box_x = (cur_mode_info.XResolution - 400) / 2;
    int box_y = cur_mode_info.YResolution - 80;
    draw_rectangle(box_x, box_y, 400, 30, 0x1E1E2E);
    draw_xpm_sentence(cur_typed_word, box_x + 8, box_y + 8, "default");
    return 0;
}

int (code_to_word)() {
    int len = strlen(cur_typed_word);
    if (len < MAX_WORD_SIZE - 1) {
        switch (cur_scancode) {
            case 0x39: if(len == 0) return 0; return 1;
            case 0x1E: cur_typed_word[len] = 'a'; return 0;
            case 0x30: cur_typed_word[len] = 'b'; return 0;
            case 0x2E: cur_typed_word[len] = 'c'; return 0;
            case 0x20: cur_typed_word[len] = 'd'; return 0;
            case 0x12: cur_typed_word[len] = 'e'; return 0;
            case 0x21: cur_typed_word[len] = 'f'; return 0;
            case 0x22: cur_typed_word[len] = 'g'; return 0;
            case 0x23: cur_typed_word[len] = 'h'; return 0;
            case 0x17: cur_typed_word[len] = 'i'; return 0;
            case 0x24: cur_typed_word[len] = 'j'; return 0;
            case 0x25: cur_typed_word[len] = 'k'; return 0;
            case 0x26: cur_typed_word[len] = 'l'; return 0;
            case 0x32: cur_typed_word[len] = 'm'; return 0;
            case 0x31: cur_typed_word[len] = 'n'; return 0;
            case 0x18: cur_typed_word[len] = 'o'; return 0;
            case 0x19: cur_typed_word[len] = 'p'; return 0;
            case 0x10: cur_typed_word[len] = 'q'; return 0;
            case 0x13: cur_typed_word[len] = 'r'; return 0;
            case 0x1f: cur_typed_word[len] = 's'; return 0;
            case 0x14: cur_typed_word[len] = 't'; return 0;
            case 0x16: cur_typed_word[len] = 'u'; return 0;
            case 0x2f: cur_typed_word[len] = 'v'; return 0;
            case 0x11: cur_typed_word[len] = 'w'; return 0;
            case 0x2d: cur_typed_word[len] = 'x'; return 0;
            case 0x15: cur_typed_word[len] = 'y'; return 0;
            case 0x2c: cur_typed_word[len] = 'z'; return 0;
            default: break;
        }
    } else return -1;
    return 0;
}

int (word_checker)(int n) {
    if(strcmp(word_list[n].word, cur_typed_word) == 0) {
        word_list[n].state = CORRECT;
        return 0;
    } else {
        word_list[n].state = WRONG;
        return 1;
    }
}

void (word_scrambler)() {
    for(int i = 0; i < MAX_GAME_WORDS; i++) {
        int word_bank = i % 3 + 1;
        int random_index = rand() % 491;
        struct words next_word;
        switch (cur_game_lang) {
            case ENG:
                if(word_bank == 1) strcpy(next_word.word, eng_word_bank1[random_index]);
                else if(word_bank == 2) strcpy(next_word.word, eng_word_bank2[random_index]);
                else strcpy(next_word.word, eng_word_bank3[random_index]);
                break;
            case PT:
                if(word_bank == 1) strcpy(next_word.word, pt_word_bank1[random_index]);
                else if(word_bank == 2) strcpy(next_word.word, pt_word_bank2[random_index]);
                else strcpy(next_word.word, pt_word_bank3[random_index]);
                break;
            case ESP:
                if(word_bank == 1) strcpy(next_word.word, esp_word_bank1[random_index]);
                else if(word_bank == 2) strcpy(next_word.word, esp_word_bank2[random_index]);
                else strcpy(next_word.word, esp_word_bank3[random_index]);
                break;
            default: break;
        }
        next_word.state = NOTCHECKED;
        word_list[i] = next_word;
    }
}

int (main_interrupt_handler)() {
    if (set_frame_buffer(0x114) != 0 || set_graphic_mode(0x114) != 0) return 1;
    int ipc_status;
    uint8_t irq_keyboard, irq_timer = 0;
    message msg;

    int cur_word_count = 0, correct_words = 0, wrong_words = 0, final_time = 0;
    int game_time = 15, last_game_time = game_time;
    enum gamestate game_state = WAITING;

    word_scrambler();
    if (keyboard_subscribe_int(&irq_keyboard)!=0 || timer_subscribe_int(&irq_timer)!=0) return 1;

    draw_static_background();
    draw_words();
    draw_textbox_content();
    swap_buffers();

    while(game_state != EXIT) {
        int r;
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) continue;

        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    if(msg.m_notify.interrupts & irq_timer & (game_state == STARTED)) {
                        timer_int_handler();
                        if (timer_counter % 60 == 0) game_time--;
                        draw_textbox_content();
                        swap_buffers();
                    }
                    if (msg.m_notify.interrupts & irq_keyboard) {
                        kbc_ih();
                        if(game_state != STATS) {
                            game_state = STARTED;
                            if ((cur_scancode & BREAK_CODE) == 0) {
                                int result = code_to_word();
                                if (result == -1) result = 1;
                                if (result == 1) {
                                    if (word_checker(cur_word_count) == 0) correct_words++;
                                    else wrong_words++;
                                    memset(cur_typed_word, 0, sizeof(cur_typed_word));
                                    cur_word_count++;
                                    draw_words();
                                }
                                draw_textbox_content();
                                swap_buffers();
                            }
                        }
                    }
                    break;
                default: break;
            }
        }

        if (game_state == STATS) {
            int percent = (100 * correct_words) / cur_word_count;
            int decimal = (1000 * correct_words) / cur_word_count % 10;
            int used_time = last_game_time - final_time;
            printf("wrong words: %d\n", wrong_words);
            printf("correct words: %d\n", correct_words);
            printf("used time: %d\n", used_time);
            printf("wpm: %d\n", (cur_word_count*60/used_time));
            printf("accuracy: %d.%d%%\n", percent, decimal);
            wrong_words = correct_words = cur_word_count = final_time = 0;
            game_time = last_game_time;
            word_scrambler();
            game_state = RESETING;
            draw_static_background();
            draw_words();
            draw_textbox_content();
            swap_buffers();
        }

        if (game_state == RESETING && cur_scancode == 0x39) game_state = WAITING;
        if ((game_time == 0 && game_state == STARTED) || (cur_word_count == MAX_GAME_WORDS)) {
            game_state = STATS;
            final_time = game_time;
        }
        if (cur_scancode == BREAK_ESQ) game_state = EXIT;
    }

    vg_exit();
    printf("\033[2J\033[H");
    if (timer_unsubscribe_int()!=0 || keyboard_unsubscribe_int()!=0) return 1;
    return 0;
}

int(proj_main_loop)(int argc, char*argv[]){
    return main_interrupt_handler();
}
