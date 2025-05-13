#include "kbc.h"


//this might need to be changed in order to also support the mouse


/*
Handles keyboard interrupts (C implementation)

Reads the status register and the output buffer (OB).
If there was some error, the byte read from the OB should be discarded.

All communication with other code must be done via global variables, static if possible.


ESC = 0x81
two byte codes
    1st is usually 0xE0

this might be important IRQ_REENABLE|IRQ_EXCLUSIVE for sys_irqsetpolicy()


kbd_print_scancode()  EXISTS, IS IMPLEMENTED AND SHOULD BE USED 
    Prints the input scancode.
kbd_print_no_sysinb() EXISTS, IS IMPLEMENTED AND SHOULD BE USED
    Prints the no. of sys_inb() calls.
*/

int (get_kbc_output)(uint8_t *output){
    uint8_t status;
    uint8_t max_attempts = 10;

    while (max_attempts > 0){

        if (util_sys_inb(STATUS_REG, &status)!=0) return 1;

        if ((status & BIT(0))!=0){

            if ((status & (BIT(7)|BIT(6))) != 0) return 1; //handling parity and timeout errors

            if ((status & BIT(5)) !=0 ) return 1; //waiting for the mouse (?) me thinks

            if (util_sys_inb(OUT_BUF,output)!=0) return 1;

            return 0;
        }
 
        tickdelay(micros_to_ticks(20000));
        max_attempts--;
    }
    return 1;
}
