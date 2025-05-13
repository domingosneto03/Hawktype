#include <lcom/lcf.h>

#include <lcom/lab3.h>
#include <stdbool.h>
#include <stdint.h>

#include "i8042.h"
#include "keyboard.h"
#include "timer.c"
#include "i8254.h"

extern uint8_t cur_scancode;
extern uint32_t call_counter;
extern int timer_counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  int ipc_status;
  uint8_t irq_set;
  message msg;

  if (keyboard_subscribe_int(&irq_set)!=0) return 1;
  while(cur_scancode != BREAK_ESQ) {

    /* Get a request message. */
    int r;
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */				
          if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
            kbc_ih();

            uint8_t make;
            int num_bytes; 

            if((cur_scancode & BREAK_CODE) == 0) make = 1;
            else make = 0;

            if(cur_scancode == TWO_BYTE_CODE) num_bytes = 2;
            else num_bytes = 1;

            if (kbd_print_scancode(make,num_bytes, &cur_scancode) != 0) 
              perror("error priting scancode\n");

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
  if (keyboard_unsubscribe_int()!=0) return 1;
  if (kbd_print_no_sysinb(call_counter)!=0) return 1;
  return 0;
}

int(kbd_test_poll)() {

  while(cur_scancode != BREAK_ESQ) {
    if(get_kbc_output(&cur_scancode)==0){
      uint8_t make;
      int num_bytes; 

      if((cur_scancode & BREAK_CODE) == 0) make = 1;
      else make = 0;

      if(cur_scancode == TWO_BYTE_CODE) num_bytes = 2;
      else num_bytes = 1;

      if (kbd_print_scancode(make,num_bytes, &cur_scancode) != 0) 
        perror("error priting scancode\n");
    }
  }

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {

  int ipc_status;
  uint8_t irq_set_keyboard, irq_set_timer ;
  message msg;
  uint8_t seconds = 0;
  if (keyboard_subscribe_int(&irq_set_keyboard)!=0) return 1;
  if (timer_subscribe_int(&irq_set_timer)!=0) return 1;
  while(cur_scancode != BREAK_ESQ && seconds < n) {

    /* Get a request message. */
    int r;
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */				
          if (msg.m_notify.interrupts & irq_set_keyboard) { /* subscribed interrupt */
            kbc_ih();

            uint8_t make;
            int num_bytes; 

            if((cur_scancode & BREAK_CODE) == 0) make = 1;
            else make = 0;

            if(cur_scancode == TWO_BYTE_CODE) num_bytes = 2;
            else num_bytes = 1;

            if (kbd_print_scancode(make,num_bytes, &cur_scancode) != 0) 
              perror("error priting scancode\n");

            timer_counter = 0;
            seconds = 0;
          }
          if (msg.m_notify.interrupts & irq_set_timer) {
              timer_int_handler();
              if (timer_counter % 60 == 0) seconds++;
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
  if (timer_unsubscribe_int()!=0) return 1;
  if (keyboard_unsubscribe_int()!=0) return 1;
  if (kbd_print_no_sysinb(call_counter)!=0) return 1;
  return 0;
}
