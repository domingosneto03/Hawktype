#include <lcom/lcf.h>
#include <lcom/keyboard.h>

#include <stdint.h>

#include "i8254.h"

void (kbc_ih)(void) {

    uint8_t value_sat_reg;
    if (util_sys_inb(STATUS_REG,value_sat_reg) !=0) return;
    //Read Status Register and error handling

    if(BIT(7) & value_sat_reg != 0 || BIT(6) & value_sat_reg != 0) return;
    //checking for errors/invalid data

    if(BIT(0) & value_sat_reg == 0) return;
    //checking if there's data to be read
    
    //Read Output Buffer
    

    //Error Handling

    //put codes in global variables
}

/*
Handles keyboard interrupts (C implementation)

Reads the status register and the output buffer (OB).
If there was some error, the byte read from the OB should be discarded.

All communication with other code must be done via global variables, static if possible.
*/

/*
ESC = 0x81
two byte codes
    1st is usually 0xE0

this might be important IRQ_REENABLE|IRQ_EXCLUSIVE for sys_irqsetpolicy()
*/

/*
kbd_print_scancode()  EXISTS, IS IMPLEMENTED AND SHOULD BE USED 
    Prints the input scancode.
kbd_print_no_sysinb() EXISTS, IS IMPLEMENTED AND SHOULD BE USED
    Prints the no. of sys_inb() calls.
*/