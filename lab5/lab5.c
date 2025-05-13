// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>
#include "graphics.h"
#include <stdint.h>
#include <stdio.h>
#include "keyboard.h"
#include "kbc.h"
#include "timer.c"

extern uint8_t cur_scancode;
extern vbe_mode_info_t cur_mode_info;

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (ESC_pressed)(){
  int ipc_status;
  message msg;
  uint8_t keyboard_id;

  if (keyboard_subscribe_int(&keyboard_id)!=0){
    return 1;
  }

  while (cur_scancode != BREAK_ESQ){
    int r;
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */				
          if (msg.m_notify.interrupts & keyboard_id) {
            kbc_ih();
            break;
          } 
        default:
          break;
      }
    }
  }
  if (keyboard_unsubscribe_int()!=0){
    return 1;
  }
  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  
  if (set_graphic_mode(mode)!=0){
    return 1;
  } 
  sleep(delay);
  
  if(vg_exit()!=0){
    return 1;
  }
  
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  
  if (set_frame_buffer(mode)!=0){
    return 1;
  }                         

  if (set_graphic_mode(mode)!=0){
    return 1;
  } 

  uint32_t normalized_color;
  if (normalization_color(color, &normalized_color) !=0){
    return 1;
  }

  if(draw_rectangle(x,y,width,height,normalized_color)!=0){
    return 1;
  }

  if (ESC_pressed() != 0 ){
    return 1;
  }

  if(vg_exit()!=0){
    return 1;
  }

  return 1;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  
  if (set_frame_buffer(mode)!=0){
    return 1;
  }                         

  if (set_graphic_mode(mode)!=0){
    return 1;
  } 

  int x_res = cur_mode_info.XResolution / no_rectangles;
  int y_res = cur_mode_info.YResolution / no_rectangles;

  uint32_t color;

  for (int i = 0; i < no_rectangles; i++ ){
    for (int j = 0; j < no_rectangles; j++ ){
      

      if(cur_mode_info.MemoryModel == DIRECT_COLOR){
        direct_mode(j, i, first, step, &color);
      }
      else{
        indexed_mode_color(j, i, step, first, no_rectangles, &color);
      }

      if (((no_rectangles-1) == i) && ((no_rectangles-1) == j) ){
        printf("%d", color);
      }

      if(draw_rectangle(j*x_res,i*y_res,x_res,y_res,color)!=0){
        return 1;
      }
    }
  }


  if (ESC_pressed() != 0 ){
    return 1;
  }

  if(vg_exit()!=0){
    return 1;
  }
  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  if (set_frame_buffer(VBE_768p_INDEXED)!=0){
    return 1;
  }                         

  if (set_graphic_mode(VBE_768p_INDEXED)!=0){
    return 1;
  } 

  if(xpm_image_to_screen(xpm, x, y)!=0){
    return 1;
  }

  if (ESC_pressed() != 0 ){
    return 1;
  }

  if(vg_exit()!=0){
    return 1;
  }
  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  
  int ipc_status;
  message msg;
  uint8_t irq_timmer;
  uint8_t irq_keyboard;

  int is_verical;
  if(xi == xf && yi < yf){
    is_verical = 1;
  }
  else if(yi == yf && xi < yf){
    is_verical = 0;
  }
  else{
    return 1;
  }

  if (keyboard_subscribe_int(&irq_keyboard)!=0) return 1;
  if (timer_subscribe_int(&irq_timmer)!=0) return 1;

  if(timer_set_frequency(0, fr_rate)!=0) return 1;

  if (set_frame_buffer(VBE_768p_INDEXED)!=0){
    return 1;
  }                         

  if (set_graphic_mode(VBE_768p_INDEXED)!=0){
    return 1;
  } 

  if(xpm_image_to_screen(xpm, xi, yi)!=0){
    return 1;
  }

  uint16_t xa = xi;
  uint16_t ya = yi;

  while(cur_scancode != BREAK_ESQ && (xa < xf || ya < yf)){
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
          }

          if (msg.m_notify.interrupts & irq_timmer){

            if(draw_rectangle(xa,ya,150,150,0xFFFFFF)!=0) return 1;

            if (is_verical){
              ya = ya + speed;
              if (ya > yf) {
                ya = yf;
              }
            }
            else{
              xa = xa + speed;
              if (xa > xf) {
                xa = xf;
              }
            }

            if(xpm_image_to_screen(xpm, xa, ya)!=0){
              return 1;
            }
            
          }		
          
          break;
        }
    }
  }

  if(vg_exit()!=0){
    return 1;
  }

  if (timer_unsubscribe_int()!=0) return 1;
  if (keyboard_unsubscribe_int()!=0) return 1;


  return 0;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}


struct words
{
  palavra = "palavra";
  state = correct/wrong/notchecked
};
