#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
                          
  switch(field){
    case tsf_all:
      printf("%0x\n",st); 
      /* Display status byte, in hexadecimal */
      break;
    case tsf_initial:
      int x = (st & (TIMER_LSB | TIMER_MSB))>>4;
      if(x==1){
        printf("LSB\n");
      }
      else if(x==2){
        printf("MSB\n");
      }
      else if(x==3){
        printf("LSB followed by MSB\n");
      }
      /* Display the initialization mode, only */
      break;
    case tsf_mode:
    /* Display the counting mode, only */
      break;
    case tsf_base:
    /* Display the counting base, only */
      break;
    default: 
      return 1;
  }
  
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}
