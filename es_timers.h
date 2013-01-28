/****************************************************************************
 Module
         ES_Timers.h

 Revision
         1.0.1

 Description
         Header File for the ME218 Timer Module

 Notes


 History
 When           Who	What/Why
 -------------- ---	--------
 01/15/12 16:43 jec  converted for Gen2 of the Events & Services Framework
 10/21/11 18:33 jec  Begin conversion for use with the new Event Framework
 09/01/05 12:29 jec  Converted rates and return values to enumerated constants
 06/15/04 09:58 jec  converted all prefixes to EF_Timer
 05/27/04 15:04 jec  revised TMR_RATE macros for the S12
 11/24/00 13:17 jec  revised TMR_RATE macros for the HC12
 02/20/99 14:11 jec  added #defines for TMR_ERR, TMR_ACTIVE, TMR_EXPIRED
 02/24/97 14:28 jec  Began Coding
****************************************************************************/

#ifndef ES_Timers_H
#define ES_Timers_H

#include "Bin_Const.h"
#include "ES_Types.h"

/* these assume an 8MHz OSCCLK, they are the values to be used to program
    the RTICTL regiser
 */
typedef enum { ES_Timer_RATE_OFF  =   (0),
               ES_Timer_RATE_1MS  = B8(00100011), /* 1.024mS */
               ES_Timer_RATE_2MS  = B8(00110011), /* 2.048mS */
               ES_Timer_RATE_4MS  = B8(01000011), /* 4.096mS */
               ES_Timer_RATE_8MS  = B8(01010011), /* 8.196mS */
               ES_Timer_RATE_16MS = B8(01100011), /* 16.384mS */
               ES_Timer_RATE_32MS = B8(01110011)  /* 32.768mS */
} TimerRate_t;

typedef enum { ES_Timer_ERR           = -1,
               ES_Timer_ACTIVE        =  1,
               ES_Timer_OK            =  0,
               ES_Timer_NOT_ACTIVE    =  0
} ES_TimerReturn_t;

void             ES_Timer_Init(TimerRate_t Rate);
ES_TimerReturn_t ES_Timer_InitTimer(unsigned char Num, unsigned int NewTime);
ES_TimerReturn_t ES_Timer_SetTimer(unsigned char Num, unsigned int NewTime);
ES_TimerReturn_t ES_Timer_StartTimer(unsigned char Num);
ES_TimerReturn_t ES_Timer_StopTimer(unsigned char Num);
ES_TimerReturn_t ES_Timer_IsTimerActive(unsigned char Num);
uint16_t     ES_Timer_GetTime(void);

#endif   /* ES_Timers_H */
/*------------------------------ End of file ------------------------------*/


