#include "keyboard.h"

uint8_t cur_scancode = 0;
int hook_id_keyboard = 1; 

int (keyboard_subscribe_int)(uint8_t *bit_no) {

    if (bit_no==NULL) {return 1;}
    *bit_no = (uint8_t)BIT((int)hook_id_keyboard); 
    if (sys_irqsetpolicy(IRQ_KEYBOARD, IRQ_REENABLE|IRQ_EXCLUSIVE, &hook_id_keyboard)!=0) return 1;
    return 0;
  }

int (keyboard_unsubscribe_int)() {
    if(sys_irqrmpolicy(&hook_id_keyboard)!=0) return 1;
    return 0;
  }


void (kbc_ih)(){
    if (get_kbc_output(&cur_scancode, 0)!=0) perror("error getting output\n");
}
