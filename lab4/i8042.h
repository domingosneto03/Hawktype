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
#define IRQ_MOUSE 12

/* MOUSE LAB */

#define MOUSE_LEFT_BUTTON BIT(0)
#define MOUSE_RIGHT_BUTTON BIT(1)
#define MOUSE_MIDDLE_BUTTON BIT(2)
#define FIRST_BYTE_RECEIVED BIT(3)
#define NEGATIVE_X BIT(4)
#define NEGATIVE_Y BIT(5)
#define OVERFLOW_X BIT(6)
#define OVERFLOW_Y BIT(7)

#define MOUSE_REQUEST 0xD4



/**@}*/

#endif 
