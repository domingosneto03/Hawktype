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

enum gamestate{
    WAITING,    //waiting to start (clickar tecla)
    STARTED,    //timer a contar 
    STATS,      //pós game a mostrar stats, à espera que se saia/comece novo jogo
    RESETING,   //espera pelo o reset do jogo
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

int (code_to_word)() {
    int len = strlen(cur_typed_word);

    switch (cur_scancode) {
        case 0x0E:  // Backspace
            if (len > 0) {
                cur_typed_word[len - 1] = '\0';
            }
            return 0;
        case 0x39:  // Spacebar
            if (len == 0) return 0;  // Ignore space if nothing is typed
            return 1;  // End of word
        default:
            break;
    }

    if (len >= MAX_WORD_SIZE - 1) {
        return -1;  // Limit reached
    }

    char c = 0;
    switch (cur_scancode) {
        case 0x1E: c = 'a'; break;
        case 0x30: c = 'b'; break;
        case 0x2E: c = 'c'; break;
        case 0x20: c = 'd'; break;
        case 0x12: c = 'e'; break;
        case 0x21: c = 'f'; break;
        case 0x22: c = 'g'; break;
        case 0x23: c = 'h'; break;
        case 0x17: c = 'i'; break;
        case 0x24: c = 'j'; break;
        case 0x25: c = 'k'; break;
        case 0x26: c = 'l'; break;
        case 0x32: c = 'm'; break;
        case 0x31: c = 'n'; break;
        case 0x18: c = 'o'; break;
        case 0x19: c = 'p'; break;
        case 0x10: c = 'q'; break;
        case 0x13: c = 'r'; break;
        case 0x1F: c = 's'; break;
        case 0x14: c = 't'; break;
        case 0x16: c = 'u'; break;
        case 0x2F: c = 'v'; break;
        case 0x11: c = 'w'; break;
        case 0x2D: c = 'x'; break;
        case 0x15: c = 'y'; break;
        case 0x2C: c = 'z'; break;
        default:
            return 0;
    }

    cur_typed_word[len] = c;
    cur_typed_word[len + 1] = '\0';  // Always null terminate
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

int draw_initial_screen() {

    draw_rectangle(0, 0, cur_mode_info.XResolution, cur_mode_info.YResolution, 0x1E1E2E);


    // Title
    draw_xpm_title("HAWKTYPE", 10, 10);

    int margin_x = 60;
    int line_spacing = 30;
    int word_spacing = 15;
    int cursor_x = margin_x;
    int cursor_y = 200;
    int max_x = cur_mode_info.XResolution - margin_x;
    int num_words = 25;

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

    // --- Textbox layout ---
    int box_width = 400;
    int box_height = 30;
    int box_x = (cur_mode_info.XResolution - box_width) / 2;
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


int (main_interrupt_handler)(){

    if (set_frame_buffer(0x114) != 0) return 1;
    if (set_graphic_mode(0x114) != 0) return 1;
    int ipc_status;
    uint8_t irq_keyboard;
    uint8_t irq_timer = 0; //add other iqrs as needed
    message msg;

    //variavias para estatisticas
    int cur_word_count = 0;
    int correct_words = 0;
    int wrong_words = 0;
    int final_time = 0;

    int game_time = 15;
    int last_game_time = game_time;
    //int total_words = MAX_GAME_WORDS;
    enum gamestate game_state = WAITING;

    //esta parte, creio que terá de ser feita dentro de um main loop
    //(diferente do loop  while(cur_word_count < total_words) )
    word_scrambler();

    if (keyboard_subscribe_int(&irq_keyboard)!=0) return 1;
    if (timer_subscribe_int(&irq_timer)!=0) return 1;
   // if (timer_set_frequency(0,60)!=0) return 1;

    draw_initial_screen();
    swap_buffers();


   //aqui
    for(int x = 0; x<MAX_GAME_WORDS; x++){
        printf(" %s", word_list[x].word);
    }
    printf("\n");

    while(game_state != EXIT) {
        //cur_word_count < total_words && game_time > 0 && cur_scancode != BREAK_ESQ

        int r;
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
        }

        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: /* hardware interrupt notification */				
                    
                if(msg.m_notify.interrupts & irq_timer & (game_state == STARTED)){
                        timer_int_handler(); 
                    if (timer_counter%60==0){
                        game_time--;
                        printf("%d\n",game_time);
                    }

                    // Update screen (textbox + phrase)
                    //draw_rectangle(0, 90, 1024, 100, 0x000000); // clear phrase/textbox area
                    draw_initial_screen();
                    swap_buffers();
                }


                
                if (msg.m_notify.interrupts & irq_keyboard) { /* subscribed interrupt */
                    kbc_ih();
                    if(game_state!=STATS){
                        game_state = STARTED;
                        uint8_t make;
                        //int num_bytes; 
                        int scan_handler;
                        int wrong_word;

                        if((cur_scancode & BREAK_CODE) == 0) make = 1;
                        else make = 0;

                        //desnecessário creio, mas quando copiei o loop do lab3 só comentei lol
                        // if(cur_scancode == TWO_BYTE_CODE) num_bytes = 2;
                        // else num_bytes = 1;

                        if(make){
                            scan_handler = code_to_word();

                            if(scan_handler == -1){
                                printf("Word size limit reached.");
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

                                //debug
                                printf("Word %d: %s \n", cur_word_count, cur_typed_word );
                                //limpar palavra
                                memset(cur_typed_word,0,sizeof(cur_typed_word));
                                cur_word_count++;
                            }
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
            //float cor_words = correct_words;
            int percent = (100 * correct_words) / cur_word_count;
            int decimal = (1000 * correct_words) / cur_word_count % 10;
            int used_time = last_game_time-final_time;
            printf("wrong words: %d\n", wrong_words);
            printf("correct words: %d\n", correct_words);
            printf("used time: %d\n",used_time );
            printf("wpm: %d\n", (cur_word_count*60/used_time));
            printf("cheguei aqui\n");
            printf("accuracy: %d.%d%%\n", percent, decimal);
            wrong_words = 0;    
            correct_words = 0;
            cur_word_count = 0;
            final_time = 0; //maybe desnesseário
            game_time = last_game_time;   
            word_scrambler();
            game_state = RESETING;

            //este loop será substituido por representar as palabras no ecra mas por enquanto
            for(int x = 0; x<MAX_GAME_WORDS; x++){
                printf(" %s", word_list[x].word);
            }
            printf("\n");
            
        }

        if(game_state==RESETING){
            if(cur_scancode == 0x39){     //espaço tá a dar reset ao jogo
                game_state = WAITING;    
            }
        }


        if((game_time == 0 & game_state == STARTED) || (cur_word_count==MAX_GAME_WORDS) ){
            game_state = STATS;
            final_time = game_time;
        }

        if(cur_scancode == BREAK_ESQ) game_state = EXIT; //isto será retirado (ou nao)

    }
    vg_exit();
    printf("\033[2J\033[H");

    
    //aqui
    if (timer_unsubscribe_int()!=0) return 1;
    if (keyboard_unsubscribe_int()!=0) return 1;
    return 0;
}

int(proj_main_loop)(int argc, char*argv[]){
    if (main_interrupt_handler()!=0) return -1;
    /* your main loop code here */
    return 0;
}
