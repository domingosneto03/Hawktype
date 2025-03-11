#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int hook_id = 0; 
int timer_counter = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  if (timer < 0 || timer > 2) return 1; //error handling basically

  if (freq > TIMER_FREQ || freq < 19) return 1;

  uint8_t cur_config;
  uint8_t read_back_command = (TIMER_RB_CMD | TIMER_RB_SEL(timer) | BIT(5)); 
  //this BIT(5) could be replace by a macro maybe
  if (sys_outb(TIMER_CTRL, read_back_command )!=0) return 1;
  if (util_sys_inb(TIMER_0 + timer, &cur_config)!=0) return 1;

  uint8_t MSB, LSB, timer_port;
  uint8_t control_word = TIMER_LSB_MSB | (cur_config & 0x0f);
  uint32_t timer_freq = TIMER_FREQ / freq;

  util_get_MSB(timer_freq, &MSB);
  util_get_LSB(timer_freq, &LSB);


  if (timer == 0){
    control_word |= TIMER_SEL0;
    timer_port = TIMER_0;
  }
  else if (timer == 1){
    control_word |= TIMER_SEL1;
    timer_port = TIMER_1;
  }
  else if (timer == 2){
    control_word |= TIMER_SEL2;
    timer_port = TIMER_2;
  }

  if (sys_outb(TIMER_CTRL, control_word )!=0) return 1;
  if (sys_outb(timer_port,LSB)!=0) return 1; 
  if (sys_outb(timer_port,MSB)!=0) return 1; 

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {

  if (bit_no==NULL) {return 1;}
  *bit_no = (uint8_t)BIT((int)*bit_no); //isto parece-me errado, porém funcionou
  if (sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&hook_id)!=0) return 1;

  return 0;
}

int (timer_unsubscribe_int)() {

  if(sys_irqrmpolicy(&hook_id)!=0) return 1;

  return 0;
}

void (timer_int_handler)() {
  timer_counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {

  if (timer < 0 || timer > 2 || st == NULL ) return 1; //error handling basically
  uint8_t read_back_command = (TIMER_RB_CMD | TIMER_RB_SEL(timer) | BIT(5)); 
  //this BIT(5) could be replace by a macro maybe
  if (sys_outb(TIMER_CTRL, read_back_command )!=0) return 1;
  if (util_sys_inb(TIMER_0 + timer, st)!=0) return 1;
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  
  union timer_status_field_val timer_data;

  switch(field){
    case tsf_all:

      timer_data.byte = st;
      break;

    case tsf_initial:

      {
      uint8_t x = (uint8_t)((st >> 4) & 0x03);
      //0111
      if (x==1) timer_data.in_mode = LSB_only;
      else if (x==2) timer_data.in_mode = MSB_only;
      else if (x==3) timer_data.in_mode = MSB_after_LSB;
      else timer_data.in_mode = INVAL_val;
    }
      break;

    case tsf_mode:
      {
        uint8_t x= (uint8_t)((st >> 1) & (BIT(0) | BIT(1) | BIT(2)));
        
        if(x==0) timer_data.count_mode = 0; //000 = 0
        else if(x==1) timer_data.count_mode = 1; //001 = 1
        else if(x==2 || x==6) timer_data.count_mode = 1; //010 = 2 ou 110 = 6
        else if(x==3 || x==7) timer_data.count_mode = 3; //011 = 3 ou 111 = 7
        else if(x==4) timer_data.count_mode = 4; //100 = 4
        else if(x==5) timer_data.count_mode = 5; //101 = 5
      }
      break;
    case tsf_base:
      
      timer_data.bcd = st & BIT(0);
      break;
    default: 
      return 1;

  }
  if (timer_print_config(timer, field, timer_data) != 0) return 1;

  return 0;
}
