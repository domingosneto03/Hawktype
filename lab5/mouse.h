#ifndef _LCOM_MOUSE_H_
#define _LCOM_MOUSE_H_

#include <minix/sysutil.h>
#include "i8042.h"
#include <lcom/lcf.h>
#include "KBC.h"

int (mouse_subscribe_int)(uint8_t *bit_no);

int (mouse_unsubscribe_int)();

void (mouse_ih)();

void (mouse_byte_syncing)();

void (mouse_bytes_to_packet)();

void (state_machine_update)(uint8_t x_len,uint8_t tolarance);


#endif
