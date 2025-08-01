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
#include "mouse.c"
#include "xpm_font.h"


extern vbe_mode_info_t cur_mode_info;
extern uint8_t cur_scancode;
extern int timer_counter; 
int mouse_x = 120 + 171;
int mouse_y = 124;
extern struct packet mouse_packet;
uint8_t cursor_visible = 1;
// --- Textbox layout ---
int box_width = 400;
int box_height = 30;
int game_time = 15;

char cur_typed_word[MAX_WORD_SIZE] = "";

// -- Menu layout ---
const int menu_x = 80;
const int menu_y = 120;
const int slogan_start_x = 220;
const int slogan_start_y = 90;
const int time_title_x = menu_x + 30;
const int time_title_y = menu_y;
const int language_title_x = menu_x + 170;
const int language_title_y = menu_y;
const int button_15_x = menu_x + 30;
const int button_30_x = menu_x + 70;
const int button_60_x = menu_x + 110;
const int button_eng_x = menu_x + 170;
const int button_por_x = menu_x + 240;
const int button_esp_x = menu_x + 310;
const int button_y = menu_y + 30;

enum gamestate{
    WAITING,   
    STARTED,   
    STATS,      
    RESETING,   
    EXIT
};

enum wordstate{
    NOTCHECKED,
    CORRECT,
    WRONG,
};


enum game_languages{
    ENG,
    PT,
    ESP
};

enum game_languages cur_game_lang = ENG;


struct words{
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

  // enables to save the output of //printf function calls on a file
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
            case 0x0E:
                if (len!=0){
                    cur_typed_word[len-1] = 0x00;
                }
                return 0;
            case 0x39:
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
        word_list[n].state = CORRECT;
        return 0;
    }
    else{
        word_list[n].state = WRONG;
        return 1;
    }
    return 0;
}

void (word_scrambler)(){
    for(int i = 0; i < MAX_GAME_WORDS; i++){

        //fazer com que a cada iteração escolha um dos 3 word_banks
        //vai ser sempre : 1 2 3 1 2 3 1 2 3
        int word_bank = i % 3 + 1;
        int random_index = rand() % 491;
        struct words next_word;
        
        switch (cur_game_lang){
            case ENG:
                if(word_bank == 1){
                    strcpy(next_word.word, eng_word_bank1[random_index]);
                    next_word.state = NOTCHECKED;
                }
                else if(word_bank == 2){
                    strcpy(next_word.word, eng_word_bank2[random_index]);
                    next_word.state = NOTCHECKED;
                }
                else if(word_bank == 3){
                    strcpy(next_word.word, eng_word_bank3[random_index]);
                    next_word.state = NOTCHECKED;
                }
                word_list[i] = next_word;
                break;

            case PT:
                if(word_bank == 1){
                    strcpy(next_word.word, pt_word_bank1[random_index]);
                    next_word.state = NOTCHECKED;
                }
                else if(word_bank == 2){
                    strcpy(next_word.word, pt_word_bank2[random_index]);
                    next_word.state = NOTCHECKED;
                }
                else if(word_bank == 3){
                    strcpy(next_word.word, pt_word_bank3[random_index]);
                    next_word.state = NOTCHECKED;
                }
                word_list[i] = next_word;
                break;

            case ESP:
                if(word_bank == 1){
                    strcpy(next_word.word, esp_word_bank1[random_index]);
                    next_word.state = NOTCHECKED;
                }
                else if(word_bank == 2){
                    strcpy(next_word.word, esp_word_bank2[random_index]);
                    next_word.state = NOTCHECKED;
                }
                else if(word_bank == 3){
                    strcpy(next_word.word, esp_word_bank3[random_index]);
                    next_word.state = NOTCHECKED;
                }
                word_list[i] = next_word;
                break;
            default:
                break;
            }
    
    }
}

int draw_mouse_cursor() {
    return draw_xpm_cursor(mouse_x, mouse_y);
}

const int margin_x = 60;
const int line_spacing = 30;
const int word_spacing = 15;
const int num_words = 25;

int draw_sentence(){

    int cursor_x = margin_x;
    int cursor_y = 200;
    int max_x = cur_mode_info.XResolution - margin_x;

    for (int i = 0; i < num_words; i++) {
        int word_width = strlen(word_list[i].word) * 15;

        if (cursor_x + word_width > max_x) {
            cursor_x = margin_x;
            cursor_y += line_spacing;
        }

        draw_xpm_sentence(word_list[i].word, cursor_x, cursor_y, "default");
        switch (word_list[i].state) {
            case CORRECT: draw_xpm_sentence(word_list[i].word, cursor_x, cursor_y, "green"); break;
            case WRONG: draw_xpm_sentence(word_list[i].word, cursor_x, cursor_y, "red"); break;
            case NOTCHECKED:
            default: break;
        }

        cursor_x += word_width + word_spacing;
    }

    return 0;
}
  

int draw_input() {
    int box_x = (cur_mode_info.XResolution - box_width) / 3;
    int box_y = cur_mode_info.YResolution - 80; // e.g. 500 for 600p

    // Label
    draw_xpm_sentence("type", box_x - 80, box_y + 4, "default"); // light gray label

    // Textbox outline
    draw_rectangle(box_x - 2, box_y - 2, box_width + 4, box_height + 4, 0xDDDDDD);

    // Textbox background
    draw_rectangle(box_x, box_y, box_width, box_height, 0x1E1E2E);

    // User text
    draw_xpm_sentence(cur_typed_word, box_x + 8, box_y + 8, "default");

    return 0;
}


int draw_top_menu() {
    
    draw_xpm_sentence("TEST YOUR TYPING SKILLS", slogan_start_x, slogan_start_y, "default");

    draw_xpm_sentence("Time", time_title_x, time_title_y, "default");
    draw_button_15(button_15_x, button_y);
    draw_button_30(button_30_x, button_y); 
    draw_button_60(button_60_x, button_y);

    draw_xpm_sentence("Language", menu_x + 170, 120, "default");
    draw_button_eng(button_eng_x, button_y);
    draw_button_por(button_por_x, button_y);
    draw_button_esp(button_esp_x, button_y);

    return 0;
}

int draw_timer(int game_time) {

    draw_xpm_sentence("Time left: ", 500, menu_y, "default");
    draw_xpm_numbers(game_time, 500, button_y);

    return 0;
}


bool button_15_clicked = false;
bool button_30_clicked = true;
bool button_60_clicked = false;

int time_button_handler(int x, int y) {

    if (x > button_15_x && x < button_15_x + 60 &&
        y > button_y && y < button_y + 30) {
        if(!button_15_clicked) {
            if (mouse_packet.lb) {
                game_time = 15; 
                button_15_clicked = true;
                button_30_clicked = false;
                button_60_clicked = false;
                printf("15 seconds selected\n");
                printf("Game time set to %d seconds\n", game_time);
                draw_timer(game_time);
            }
        }
    } else if (x > button_30_x && x < button_30_x + 60 &&
               y > button_y && y < button_y + 30) {
        if(!button_30_clicked) {
            if (mouse_packet.lb) {
                game_time = 30; 
                button_15_clicked = false;
                button_30_clicked = true;
                button_60_clicked = false;
                printf("30 seconds selected\n");
                printf("Game time set to %d seconds\n", game_time);
                draw_timer(game_time);
            }
        }
    } else if (x > button_60_x && x < button_60_x + 60 &&
               y > button_y && y < button_y + 30) {
            if (!button_60_clicked) {
                if(mouse_packet.lb) {
                    game_time = 60; 
                    button_15_clicked = false;
                    button_30_clicked = false;
                    button_60_clicked = true;
                    printf("60 seconds selected\n");
                    draw_timer(game_time);
                }
            }
    
    } else {
        return -1; // Invalid click
    }

    return 0;
}

int button_eng_clicked = false;
int button_por_clicked = true;
int button_esp_clicked = false;

int language_button_handler(int x, int y, int last_game_time) {

    if (x > button_eng_x && x < button_eng_x + 70 &&
        y > button_y && y < button_y + 30) {
        if(!button_eng_clicked) {
            if (mouse_packet.lb) {
                cur_game_lang = ENG;
                button_eng_clicked = true;
                button_por_clicked = false;
                button_esp_clicked = false;
                printf("English selected\n");
                printf("Language set to English\n");
                word_scrambler();
                draw_sentence();
                game_time = last_game_time; 
            }
        }
    } else if (x > button_por_x && x < button_por_x + 70 &&
            y > button_y && y < button_y + 30) {
            if(!button_por_clicked) {
                if (mouse_packet.lb) {
                    cur_game_lang = PT;
                    button_eng_clicked = false;
                    button_por_clicked = true;
                    button_esp_clicked = false;
                    printf("Portuguese selected\n");
                    printf("Language set to Portuguese\n");
                    word_scrambler();
                    draw_sentence();
                    game_time = last_game_time;
                }
            }
    } else if (x > button_esp_x && x < button_esp_x + 70 &&
               y > button_y && y < button_y + 30) {
        if(!button_esp_clicked) {
            if (mouse_packet.lb) {
                cur_game_lang = ESP;
                button_eng_clicked = false;
                button_por_clicked = false;
                button_esp_clicked = true;
                printf("Spanish selected\n");
                printf("Language set to Spanish\n");
                word_scrambler();
                draw_sentence();
                game_time = last_game_time;
            }
        }
    } else {
        return -1; // Invalid click
    }

    return 0;
}

int draw_game_stats( int correct_words, int percent, int decimal, int cur_wpm, int accuracy, int used_time, int wrong_words){ 

    int box_x = (cur_mode_info.XResolution - box_width) / 2;
    int box_y = cur_mode_info.YResolution - 80; // e.g. 500 for 600p

    // Label
    draw_xpm_sentence("Game Stats", box_x - 80, box_y + 4, "default"); // light gray label

    // Textbox outline
    draw_rectangle(box_x - 2, box_y - 2, box_width + 4, box_height + 4, 0xDDDDDD);

    // Textbox background
    draw_rectangle(box_x, box_y, box_width, box_height, 0x1E1E2E);

    // Display stats
    char stats[100];
    sprintf(stats, "Time: %d seconds", game_time);
    draw_xpm_sentence(stats, box_x + 8, box_y + 8, "default");

    sprintf(stats, "Words Typed: %d", MAX_GAME_WORDS);
    draw_xpm_sentence(stats, box_x + 8, box_y + 28, "default");

    sprintf(stats, "Correct Words: %d", correct_words);
    draw_xpm_sentence(stats, box_x + 8, box_y + 48, "default");

    sprintf(stats, "Wrong Words: %d", wrong_words);
    draw_xpm_sentence(stats, box_x + 8, box_y + 68, "default");

    return 0;
}



int draw_initial_screen(int game_time) {

    draw_rectangle(0, 0, cur_mode_info.XResolution, cur_mode_info.YResolution, 0x1E1E2E);

    // Title
    draw_xpm_title("HAWKTYPE", 250, 30);
    draw_top_menu();
    draw_timer(game_time);
    draw_sentence();
    draw_input();

    if (cursor_visible)
        draw_mouse_cursor();

    return 0;
}


int (main_interrupt_handler)(){

    if (set_frame_buffer(0x114) != 0) return 1;
    if (set_graphic_mode(0x114) != 0) return 1;
    int ipc_status;
    uint8_t irq_keyboard;
    uint8_t irq_timer = 0; //add other iqrs as needed
    uint8_t irq_mouse; //mouse irq
    message msg;

    //variavias para estatisticas
    int cur_word_count = 0;
    int correct_words = 0;
    int wrong_words = 0;
    int final_time = 0;

    int last_game_time = game_time;
    //int total_words = MAX_GAME_WORDS;
    enum gamestate game_state = WAITING;

    word_scrambler();

    if (keyboard_subscribe_int(&irq_keyboard)!=0) return 1;
    if (timer_subscribe_int(&irq_timer)!=0) return 1;
    if(mouse_subscribe_int(&irq_mouse)!=0) return 1;
    // if (timer_set_frequency(0,60)!=0) return 1;

    draw_initial_screen(game_time);
    swap_buffers();


   //aqui
    for(int x = 0; x<MAX_GAME_WORDS; x++){
        //printf(" %s", word_list[x].word);
    }
    //printf("\n");

    while(game_state != EXIT) {
        //cur_word_count < total_words && game_time > 0 && cur_scancode != BREAK_ESQ

        int r;
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        //printf("driver_receive failed with: %d", r);
        continue;
        }

        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: /* hardware interrupt notification */				
                    
                if(msg.m_notify.interrupts & irq_timer & (game_state == STARTED)){
                        timer_int_handler(); 
                    if (timer_counter%60==0){
                        game_time--;
                        //printf("%d\n",game_time);
                    }

                    //draw_rectangle(0, 90, 1024, 100, 0x000000); 
                    draw_initial_screen(game_time);
                    swap_buffers();
                }


                
                if (msg.m_notify.interrupts & irq_keyboard) { /* subscribed interrupt */
                    kbc_ih();
                    if (game_state == WAITING){

                        if (cur_scancode == 0x02) {
                            if (!button_15_clicked) {
                                game_time = 15; 
                                button_15_clicked = true;
                                button_30_clicked = false;
                                button_60_clicked = false;
                                printf("15 seconds selected\n");
                                printf("Game time set to %d seconds\n", game_time);
                                draw_timer(game_time);
                            }
                        } else if (cur_scancode == 0x03) {
                            if (!button_30_clicked) {
                                game_time = 30; 
                                button_15_clicked = false;
                                button_30_clicked = true;
                                button_60_clicked = false;
                                printf("30 seconds selected\n");
                                printf("Game time set to %d seconds\n", game_time);
                                draw_timer(game_time);
                            }
                        } else if (cur_scancode == 0x04) {
                            if (!button_60_clicked) {
                                game_time = 60; 
                                button_15_clicked = false;
                                button_30_clicked = false;
                                button_60_clicked = true;
                                printf("60 seconds selected\n");
                                draw_timer(game_time);
                            }
                    
                        } else if (cur_scancode == 0x05) {
                            if (!button_eng_clicked) {
                                cur_game_lang = ENG;
                                button_eng_clicked = true;
                                button_por_clicked = false;
                                button_esp_clicked = false;
                                printf("English selected\n");
                                printf("Language set to English\n");
                                word_scrambler();
                                draw_sentence();
                                game_time = last_game_time; 
                                
                            }
                        } else if (cur_scancode == 0x06) {
                            if (!button_por_clicked) {
                                cur_game_lang = PT;
                                button_eng_clicked = false;
                                button_por_clicked = true;
                                button_esp_clicked = false;
                                printf("Portuguese selected\n");
                                printf("Language set to Portuguese\n");
                                word_scrambler();
                                draw_sentence();
                                game_time = last_game_time;
                            }
                        } else if (cur_scancode == 0x07) {
                            if (!button_esp_clicked) {
                                cur_game_lang = ESP;
                                button_eng_clicked = false;
                                button_por_clicked = false;
                                button_esp_clicked = true;
                                printf("Spanish selected\n");
                                printf("Language set to Spanish\n");
                                word_scrambler();
                                draw_sentence();
                                game_time = last_game_time;
                            }
                        }
                        else if (cur_scancode == 0x39) { 
                            if (game_state == WAITING) {
                                game_state = STARTED;
                                cursor_visible = 0; // Hide cursor when game starts
                                printf("Game started with %d seconds\n", game_time);
                            }
                        }
                        
                    }
                    else if(game_state!=STATS){
                        game_state = STARTED;
                        cursor_visible = 0; // Show cursor when game starts
                        uint8_t make;
                        //int num_bytes; 
                        int scan_handler;
                        int wrong_word;

                        if((cur_scancode & BREAK_CODE) == 0) make = 1;
                        else make = 0;

                        // if(cur_scancode == TWO_BYTE_CODE) num_bytes = 2;
                        // else num_bytes = 1;

                        if(make){
                            scan_handler = code_to_word();

                            if(scan_handler == -1){
                                //printf("Word size limit reached.");
                                scan_handler = 1;
                            }
                            //caso seja um espaço
                            if(scan_handler == 1){
                                //checkar se a palavra tava certa
                                wrong_word = word_checker(cur_word_count);
                                if(wrong_word==0){
                                    correct_words++;
                                }
                                else{
                                    wrong_words++;
                                }

                                memset(cur_typed_word,0,sizeof(cur_typed_word));
                                cur_word_count++;
                            }
                        }
                    }
                }
                
                if (msg.m_notify.interrupts & irq_mouse) {
                    mouse_ih();
                    mouse_byte_syncing();

                    if (game_state != STARTED) {
                    
                        if (byte_index == 3) {
                            mouse_bytes_to_packet();
                            // mouse_print_packet(&mouse_packet);

                            time_button_handler(mouse_x, mouse_y);
                            language_button_handler(mouse_x, mouse_y, last_game_time);
                            
                            mouse_x += mouse_packet.delta_x;
                            mouse_y -= mouse_packet.delta_y;  
                            
                            if (mouse_x < 0) mouse_x = 0;
                            if (mouse_y < 0) mouse_y = 0;
                            

                            draw_initial_screen(game_time); 
                            swap_buffers();

                            byte_index = 0;
                    }
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

        if(game_state==STATS){
        
            int percent = (100 * correct_words) / cur_word_count;
            int decimal = (1000 * correct_words) / cur_word_count % 10;
            int used_time = last_game_time-final_time;
            printf("wrong words: %d\n", wrong_words);
            printf("correct words: %d\n", correct_words);
            printf("used time: %d\n",used_time );
            printf("wpm: %d\n", (cur_word_count*60/used_time));
            printf("accuracy: %d.%d%%\n", percent, decimal);

            wrong_words = 0;    
            correct_words = 0;
            cur_word_count = 0;
            final_time = 0; 
            game_time = last_game_time;   
            word_scrambler();
            game_state = RESETING;

            for(int x = 0; x<MAX_GAME_WORDS; x++){
                //printf(" %s", word_list[x].word);
            }
            //printf("\n");
            
        }

        if(game_state==RESETING){
            if(cur_scancode == 0x39){     
                game_state = WAITING;    
            }
        }


        if((game_time == 0 & game_state == STARTED) || (cur_word_count==MAX_GAME_WORDS) ){
            game_state = STATS;
            final_time = game_time;
        }

        if(cur_scancode == BREAK_ESQ) game_state = EXIT; 

    }
    vg_exit();

    free_buffers();

    
    //aqui
    if (timer_unsubscribe_int()!=0) return 1;
    if (keyboard_unsubscribe_int()!=0) return 1;
    if (mouse_unsubscribe_int()!=0) return 1;
    return 0;
}

int(proj_main_loop)(int argc, char*argv[]){
    if (main_interrupt_handler()!=0) return -1;

    return 0;
}
