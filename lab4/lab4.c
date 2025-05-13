// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include "mouse.h"
#include "timer.c"

extern struct packet mouse_packet;
extern uint8_t byte_index;
extern int timer_counter;

typedef enum{
  START,
  LEFT_BUTTON,
  FIRST_LINE,
  RIGHT_BUTTON,
  END
} draw_state_machine; 

const char* draw_state_machine_to_string(draw_state_machine state) {
  static const char* names[] = {
      "START",
      "LEFT_BUTTON",
      "FIRST_LINE",
      "RIGHT_BUTTON",
      "END"
  };
  return names[state];  
}



extern draw_state_machine cur_state;

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
  int ipc_status;
  uint8_t irq_set;
  message msg;

  if(mouse_subscribe_int(&irq_set)!=0) return 1;

  if(mouse_enable_data_reporting()!=0) return 1;
  while(cnt) {
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
          mouse_ih();
          mouse_byte_syncing();
          if (byte_index==3){
            mouse_bytes_to_packet(); 
            mouse_print_packet(&mouse_packet);
            byte_index = 0;
            cnt--;  
          }
        }
        break;
      }
    }
  }
  
  if (mouse_unsubscribe_int()!=0) return 1;
  //TODO : escrever comando para o rato para dar disable
  //if (mouse_disable_data_reporting()!=0) return 1;
  return 0;
 
}

int (mouse_test_async)(uint8_t idle_time) {
  int ipc_status;
  uint8_t irq_set_mouse;
  uint8_t irq_set_timer;
  message msg;
  if(mouse_subscribe_int(&irq_set_mouse)!=0) return 1;
  if(timer_subscribe_int(&irq_set_timer)!=0) return 1;

  if(mouse_enable_data_reporting()!=0) return 1;
  uint8_t seconds = 0;
  while(seconds < idle_time) {
    /* Get a request message. */
    int r;
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */	
          if (msg.m_notify.interrupts & irq_set_timer){
            timer_int_handler();
            if(timer_counter % 60 == 0){
              seconds++;
            }
          }			
          if (msg.m_notify.interrupts & irq_set_mouse) { /* subscribed interrupt */
            mouse_ih();
            mouse_byte_syncing();
            if (byte_index==3){
              mouse_bytes_to_packet(); 
              mouse_print_packet(&mouse_packet);
              byte_index = 0;
            }
            seconds = 0;
            timer_counter = 0; 
          }
          break;
        }
    }
  }
  if (mouse_unsubscribe_int()!=0) return 1;
  if (timer_unsubscribe_int()!=0) return 1;
  return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  int ipc_status;
  uint8_t irq_set;
  message msg;

  if(mouse_subscribe_int(&irq_set)!=0) return 1;

  if(mouse_enable_data_reporting()!=0) return 1;
  cur_state = START;
  draw_state_machine final_state = END;
  while(cur_state != final_state) {
    printf("%s\n",draw_state_machine_to_string(cur_state));
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
            mouse_ih();
            mouse_byte_syncing();
            if (byte_index==3){
              mouse_bytes_to_packet(); 
              mouse_print_packet(&mouse_packet);
              state_machine_update(x_len, tolerance);
              byte_index = 0;
            }
          }
        break;
      }
    }
  }
  printf("here\n");
  if (mouse_unsubscribe_int()!=0) return 1;
  return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}
