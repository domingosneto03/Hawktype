#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/** @defgroup i8254 i8254
 * @{
 *
 * Constants for programming the i8254 Timer. Needs to be completed.
 */

/* KEYBOARD LAB */

#define DELAY_US    20000


#define READ_COMMAND_BYTE 0x20
#define OUT_BUF 0x60
#define STATUS_REG 0x64
#define BREAK_CODE 0x80
#define BREAK_ESQ 0x81
#define TWO_BYTE_CODE 0xE0

#define IRQ_KEYBOARD 1



/**@}*/

#endif 
