#ifndef PORT_H
#define PORT_H

// these macros provide the wrappers for critical regions, where ints will be off
// but the state of the interrupt enable prior to entry will be restored.
extern unsigned char _CCR_temp;

#define EnterCritical()     { __asm pshc; __asm sei; __asm movb 1,SP+,_CCR_temp; } /* This macro saves CCR register and disables global interrupts. */
#define ExitCritical()  { __asm movb _CCR_temp, 1,-SP ; __asm pulc; } /* This macro restores CCR register saved EnterCritical(). */


#endif
