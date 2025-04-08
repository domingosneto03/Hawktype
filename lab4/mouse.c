#include "mouse.h"


int hook_id_mouse = 2;
uint8_t byte_index = 0;  
struct packet mouse_packet;
uint8_t mouse_bytes[3];   
uint8_t current_byte;
struct packet init_pos;
struct packet last_pos;
int16_t x_movement = 0;
int16_t y_movement = 0;


typedef enum{
  START,
  LEFT_BUTTON,
  FIRST_LINE,
  RIGHT_BUTTON,
  END
} draw_state_machine; 

draw_state_machine cur_state;



void (mouse_ih)(){
    if (get_kbc_output(&current_byte, 1)!=0) perror("error getting output\n");
}

int (mouse_subscribe_int)(uint8_t *bit_no) {
    if (bit_no==NULL) {return 1;}
    *bit_no = (uint8_t)BIT((int)hook_id_mouse); 
    if (sys_irqsetpolicy(IRQ_MOUSE, IRQ_REENABLE|IRQ_EXCLUSIVE, &hook_id_mouse)!=0) return 1;
    return 0;
  }

int (mouse_unsubscribe_int)() {
    if(sys_irqrmpolicy(&hook_id_mouse)!=0) return 1;
    return 0;
  }

void (mouse_byte_syncing)(){
  if (byte_index == 0 && (current_byte & FIRST_BYTE_RECEIVED)){
    mouse_bytes[byte_index] = current_byte;
    byte_index++;
  }
  else if(byte_index > 0){
    mouse_bytes[byte_index] = current_byte;
    byte_index++;
  }

}

void(mouse_bytes_to_packet)(){

  for (int i = 0 ; i < 3 ; i++) {
    mouse_packet.bytes[i] = mouse_bytes[i];
  }

  mouse_packet.lb = mouse_bytes[0] & MOUSE_LEFT_BUTTON;
  mouse_packet.rb = mouse_bytes[0] & MOUSE_RIGHT_BUTTON;
  mouse_packet.mb = mouse_bytes[0] & MOUSE_MIDDLE_BUTTON;
  mouse_packet.x_ov = mouse_bytes[0] & OVERFLOW_X;
  mouse_packet.y_ov = mouse_bytes[0] & OVERFLOW_Y;
  if(mouse_bytes[0] & NEGATIVE_X) {
    mouse_packet.delta_x = (0xFF00 | mouse_bytes[1]);
  } 
  else {
    mouse_packet.delta_x = mouse_bytes[1];
  }
  if(mouse_bytes[0] & NEGATIVE_Y) {
    mouse_packet.delta_y = (0xFF00 | mouse_bytes[2]);
  }
  else {
    mouse_packet.delta_y = mouse_bytes[2];
  }
}

void (state_machine_update)(uint8_t x_len,uint8_t tolarance){

  
  switch(cur_state){
    case START:
      if(mouse_packet.lb && !mouse_packet.rb && !mouse_packet.mb ){
        cur_state = LEFT_BUTTON;
      }
    break;

    case LEFT_BUTTON:{
      if(!mouse_packet.lb){
        if ((x_movement >= x_len) 
         && (y_movement >= 1)){
          cur_state = FIRST_LINE;
          x_movement = 0;
          y_movement = 0;
        }
        else{
          cur_state = START;
          x_movement = 0;
          y_movement = 0;
        }
      }
      else{
        if(mouse_packet.delta_x >= 0 && mouse_packet.delta_y >= 0 ){
          x_movement += mouse_packet.delta_x;
          y_movement += mouse_packet.delta_y;
        }
        else{
          if(mouse_packet.delta_x < 0 && abs(mouse_packet.delta_x) > tolarance){
            cur_state = START;
            x_movement = 0;
            y_movement = 0;
          }
          if(mouse_packet.delta_y < 0 && abs(mouse_packet.delta_y) > tolarance){
            cur_state = START;
            x_movement = 0;
            y_movement = 0;
          }
          else{
            x_movement += mouse_packet.delta_x;
            y_movement += mouse_packet.delta_y;
          }
        }
      }
    }
    break; 

    case FIRST_LINE:
      if(!mouse_packet.lb && mouse_packet.rb && !mouse_packet.mb ){
        cur_state = RIGHT_BUTTON;
      }
    break;

    case RIGHT_BUTTON:{
      if(!mouse_packet.rb){
        if ((x_movement >= x_len) 
         && (abs(y_movement) >= 1)){
          cur_state = END;
          x_movement = 0;
          y_movement = 0;
        }
        else{
          cur_state = START;
          x_movement = 0;
          y_movement = 0;
        }
      }
      else{
        if(mouse_packet.delta_x > 0 && mouse_packet.delta_y < 0 ){
          x_movement += mouse_packet.delta_x;
          y_movement += mouse_packet.delta_y;
        }
        else{
          if(mouse_packet.delta_x < 0 && abs(mouse_packet.delta_x) > tolarance){
            cur_state = START;
            x_movement = 0;
            y_movement = 0;
          }
          if(mouse_packet.delta_y > 0 && abs(mouse_packet.delta_y) > tolarance){
            cur_state = START;
            x_movement = 0;
            y_movement = 0;
          }
          else{
            x_movement += mouse_packet.delta_x;
            y_movement += mouse_packet.delta_y;
          }
        }
      }
    }
    break; 

    case END:
      return;
    break;
  }
}
