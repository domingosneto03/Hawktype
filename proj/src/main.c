#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "i8042.h"
#include "keyboard.h"
#include "graphics.h"
#include "timer.c"
#include "i8254.h"


extern vbe_mode_info_t cur_mode_info;
extern uint8_t cur_scancode;
char cur_typed_word[MAX_WORD_SIZE] = "";



enum wordstate{
    NOTCHECKED,
    CORRECT,
    WRONG,
};

struct words{
    char word[MAX_WORD_SIZE]; 
    enum wordstate state;
};

struct words word_list[5] = {
    { "hello", NOTCHECKED },
    { "world", NOTCHECKED },
    { "how",   NOTCHECKED },
    { "are",   NOTCHECKED },
    { "you",   NOTCHECKED }
};

int draw_text(const char* str, uint16_t x, uint16_t y, uint32_t color) {
    int spacing = 10;
    for (size_t i = 0; str[i] != '\0'; i++) {
        char c = str[i];
        if (c != ' ') {
            draw_char(x + i * spacing, y, c, color);
        }
    }
    return 0;
}


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

/*
 * Return Codes:
 * 0 - continuamos a aceitar letras para a palavra
 * 1 - fim de palavra, proceder a ver se esta certa
 *-1 - tamanho limite de palavra atingido, 
 * vamos processa-la como está, mas lança um aviso
 *
*/

int (code_to_word)(){
    int len = strlen(cur_typed_word);
    if (len < MAX_WORD_SIZE -1){
        switch (cur_scancode){
            //this is the case of spacebar
            case 0x39:
                //não vamos aceitar o input de uma string vazia
                if(len == 0){
                    return 0;
                }
                //palavra aceite
                return 1;
            case 0x1E:
                cur_typed_word[len] = 'a';
                return 0;
            case 0x30:
                cur_typed_word[len] = 'b';
                return 0;
            case 0x2E:
                cur_typed_word[len] = 'c';
                return 0;
            case 0x20:
                cur_typed_word[len] = 'd';
                return 0;
            case 0x12:
                cur_typed_word[len] = 'e';
                return 0;
            case 0x21:
                cur_typed_word[len] = 'f';
                return 0;
            case 0x22:
                cur_typed_word[len] = 'g';
                return 0;
            case 0x23:
                cur_typed_word[len] = 'h';
                return 0;
            case 0x17:
                cur_typed_word[len] = 'i';
                return 0;
            case 0x24:
                cur_typed_word[len] = 'j';
                return 0;
            case 0x25:
                cur_typed_word[len] = 'k';
                return 0;
            case 0x26:
                cur_typed_word[len] = 'l';
                return 0;
            case 0x32:
                cur_typed_word[len] = 'm';
                return 0;
            case 0x31:
                cur_typed_word[len] = 'n';
                return 0;
            case 0x18:
                cur_typed_word[len] = 'o';
                return 0;
            case 0x19:
                cur_typed_word[len] = 'p';
                return 0;
            case 0x10:
                cur_typed_word[len] = 'q';
                return 0;
            case 0x13:
                cur_typed_word[len] = 'r';
                return 0;
            case 0x1f:
                cur_typed_word[len] = 's';
                return 0;
            case 0x14:
                cur_typed_word[len] = 't';
                return 0;
            case 0x16:
                cur_typed_word[len] = 'u';
                return 0;
            case 0x2f:
                cur_typed_word[len] = 'v';
                return 0;
            case 0x11:
                cur_typed_word[len] = 'w';
                return 0;
            case 0x2d:
                cur_typed_word[len] = 'x';
                return 0;
            case 0x15:
                cur_typed_word[len] = 'y';
                return 0;
            case 0x2c:
                cur_typed_word[len] = 'z';
                return 0;
            default:
                break;
        }
    }
    else{
        //a palavra ta demasiado grande
        return -1;
    }
    return 0;
}

int (word_checker)(int n){
    char word_to_compare[MAX_WORD_SIZE];
    strcpy(word_to_compare, word_list[n].word);

    if(strcmp(word_to_compare, cur_typed_word)==0){
        
        return 0;
    }
    else{
        return 1;
    }
}

int draw_initial_screen() {
    if (set_frame_buffer(0x114) != 0) return 1;
    if (set_graphic_mode(0x114) != 0) return 1;

    draw_rectangle(0, 0, cur_mode_info.XResolution, cur_mode_info.YResolution, 0x1E1E2E);


    // Title
    draw_xpm_title("HAWKTYPE", 10, 10);

   // Determine total width of phrase
    int total_len = 0;
    for (int i = 0; i < 5; i++) {
        total_len += strlen(word_list[i].word);
    }
    int total_width = total_len * 10 + (5 - 1) * 15;

    int x = (((cur_mode_info.XResolution - total_width) / 2) - 150);
    int y = 200; // middle of the screen, adjust as needed

    for (int i = 0; i < 5; i++) {
        
        draw_xpm_sentence(word_list[i].word, x, y);
        //draw_xpm_text(word_list[i].word, x, y);
        
        switch (word_list[i].state) {
            case CORRECT:  draw_xpm_sentence_green(word_list[i].word, x, y); break;
            case WRONG:    draw_xpm_sentence_red(word_list[i].word, x, y); break;
            case NOTCHECKED: default: break;
        }
        x += strlen(word_list[i].word) * 24 + 35;
    }

    // --- Textbox layout ---
    int box_width = 400;
    int box_height = 30;
    int box_x = (cur_mode_info.XResolution - box_width) / 2;
    int box_y = cur_mode_info.YResolution - 80; // e.g. 500 for 600p

    // Label
    draw_xpm_sentence("type", box_x - 110, box_y + 9); // light gray label

    // Textbox outline
    draw_rectangle(box_x - 2, box_y - 2, box_width + 4, box_height + 4, 0xDDDDDD);

    // Textbox background
    draw_rectangle(box_x, box_y, box_width, box_height, 0x1E1E2E);

    // User text
    draw_xpm_sentence(cur_typed_word, box_x + 8, box_y + 8);


    return 0;
}


int (main_interrupt_handler)(){

    int ipc_status;
    uint8_t irq_keyboard; //add other iqrs as needed
    message msg;

    //tem de ser mudado ----- IMPORTANT
    int total_words = 5;
    int cur_word_count = 0;
    int correct_words = 0;
    int wrong_words = 0;

    if (keyboard_subscribe_int(&irq_keyboard)!=0) return 1;

    draw_initial_screen();

    
    while(cur_word_count < total_words && cur_scancode != BREAK_ESQ) {

        int r;
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
        }

        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: /* hardware interrupt notification */				
                if (msg.m_notify.interrupts & irq_keyboard) { /* subscribed interrupt */
                    kbc_ih();

                    uint8_t make;
                    //int num_bytes; 

                    if((cur_scancode & BREAK_CODE) == 0) make = 1;
                    else make = 0;

                    //desnecessário creio, mas quando copiei o loop do lab3 só comentei lol
                    // if(cur_scancode == TWO_BYTE_CODE) num_bytes = 2;
                    // else num_bytes = 1;

                    if (make) {
                        int scan_handler = code_to_word();

                        if (scan_handler == -1) {
                            printf("Word size limit reached.");
                        }

                        // SPACE key was pressed
                        if (scan_handler == 1) {
                            int wrong_word = word_checker(cur_word_count);

                            if (wrong_word == 0) {
                                word_list[cur_word_count].state = CORRECT;
                                correct_words++;
                            } else {
                                word_list[cur_word_count].state = WRONG;
                                wrong_words++;
                            }

                            cur_word_count++;
                            memset(cur_typed_word, 0, sizeof(cur_typed_word));
                        }

                        // Update screen (textbox + phrase)
                        draw_rectangle(0, 90, 1024, 100, 0x000000); // clear phrase/textbox area
                        draw_initial_screen();
                    }

                    

                }
                    break;
                default:
                    break; /* no other notifications expected: do nothing */	
                }
            } else { 
                /* received a standard message, not a notification */
                /* no standard messages expected: do nothing */
            }
        }
    printf("wrong words: %d\n", wrong_words);
    printf("correct words: %d\n", correct_words);

    vg_exit();
    printf("\033[2J\033[H");
    if (keyboard_unsubscribe_int()!=0) return 1;
    return 0;
}

int(proj_main_loop)(int argc, char*argv[]){
    if (main_interrupt_handler()!=0) return -1;
    /* your main loop code here */
    return 0;
}
