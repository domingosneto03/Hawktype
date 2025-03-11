#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define KBD_OUT_BUF 0x60

#define STATUS_PORT 0x64
#define KBD_OUT_BUF 0x60
#define KBD_INPT_BUF 0x60
#define KBD_CMD_BUF 0x64
#define KBD_DATA_BUF 0x60
#define PORT_A 0x60
#define PORT_B 0x61
#define OUT_BUF_FULL 0x01
#define INPT_BUF_FULL 0x2
#define SYS_FLAG 0x4H
#define CMD_DATA 0x8H
#define KEYBD_INH 0x10
#define TRANS_TMOUT 0x20
#define RCV_TMOUT 0x40
#define PARITY_EVEN 0x0
#define INH_KEYBOARD 0x10
#define KBD_ENA 0xAE
#define KBD_DIS 0xAD

// Keyboard Commands
#define KB_MENU EQU 0xF1
#define KB_ENABLE EQU 0xF4
#define KB_MAKEBREAK EQU 0xF7
#define KB_ECHO EQU 0xFE
#define KB_RESET EQU 0xFF
#define KB_LED_CMD EQU 0xED

// Keyboard responses
#define KB_OK EQU 0xAA
#define KB_ACK EQU 0xFA
#define KB_OVERRUN EQU 0xFF
#define KB_RESEND EQU 0xFE
#define KB_BREAK EQU 0xF0
#define KB_FA EQU 0x10
#define KB_FE EQU 0x20
#define KB_PR_LED EQU 0x40

#define KEYBOARD_IRQ 1




#endif