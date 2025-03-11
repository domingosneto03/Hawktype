#include <lcom/lcf.h>
#include <lcom/keyboard.h>

#include <stdint.h>
#include "i8042.h"

int hook_id = 1; 

int (kbd_subscribe_int)(uint8_t *bit_no) {

    if (bit_no==NULL) {return 1;}
    *bit_no = (uint8_t)BIT((int)*bit_no); 
    if (sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &hook_id)!=0) return 1;
    
    return 0;
  }

int (kbd_unsubscribe_int)() {

    if(sys_irqrmpolicy(&hook_id)!=0) return 1;
  
    return 0;
  }


int (kbd_get_conf)(uint8_t *st) {

    if (st == NULL ) return 1; //error handling basically
    
    uint8_t read_back_command = (TIMER_RB_CMD | TIMER_RB_SEL(timer) | BIT(5)); 
    //this BIT(5) could be replace by a macro maybe
    
    //0x64 KBD_DATA_BUF 0x60 KBD_OUT_BUF
    if (sys_outb(TIMER_CTRL, read_back_command )!=0) return 1;
    if (util_sys_inb(TIMER_0 + timer, st)!=0) return 1;
    return 0;
}

void (kbc_ih)(void) {
    


}

