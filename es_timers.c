//#define TEST
/****************************************************************************
 Module
     ES_Timers.c

 Description
     This is a module implementing  8 16 bit timers all using the RTI
     timebase

 Notes
     Everything is done in terms of RTI Ticks, which can change from
     application to application.

 History
 When           Who     What/Why
 -------------- ---     --------
 02/02/12 10:01 jec      changed headers to E128 versions
 01/16/12 09:42 jec      added some more error checking to start & init
                         funcs to prevent starting a timer with no
                         service attached or with a time of 0
 01/15/12 16:46 jec      convert to Gen2 of Events & Services framework
 10/21/11 18:26 jec      begin conversion to work with the new Event Framework
 09/01/05 13:16 jec      converted the return types and parameters to use the
                         enumerated constants from the new header.
 08/31/05 10:23 jec      converted several return value tests in the test harness
                         to use symbolic values.
 06/15/04 09:56 jec      converted all external prefixes to TMRS12 to be sure
                         that we don't have any conflicts with the old libs
 05/28/04 13:53 jec      converted for 9S12C32 processor
 12/11/02 14:53 dos      converted for ICC11V6, unadorned char needs to be
                         called out as signed char, default is now unsigned
                         for a plain char.
 11/24/99 14:45 rmo		 updated to compile under ICC11v5.
 02/24/97 17:13 jec      added new function TMR_SetTimer. This will allow one
 			             function to set up the time, while another function
            			 actually initiates the timing.
 02/24/97 13:34 jec      Began Coding
****************************************************************************/

/*----------------------------- Include Files -----------------------------*/
#include <hidef.h>         /* common defines and macros */
#include <mc9s12e128.h>     /* derivative information */
#include <S12E128bits.h>    /* bit definitions  */
#include <bitdefs.h>
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_ServiceHeaders.h"
#include "ES_General.h"
#include "ES_Events.h"
#include "ES_PostList.h"
#include "ES_LookupTables.h"
#include "ES_Timers.h"
/*--------------------------- External Variables --------------------------*/

/*----------------------------- Module Defines ----------------------------*/
#define BITS_PER_BYTE 8

/*------------------------------ Module Types -----------------------------*/

/*
   the size of Tflag sets the number of timers, char = 8, int =16 ...)
   to add more timers, you will need to change the data type and modify
   the initializaiton of TMR_TimerArray and TMR_MaskArray
*/

typedef uint8_t Tflag_t;

typedef uint16_t Timer_t;


/*---------------------------- Module Functions ---------------------------*/
void EF_Timer_RTI_Resp(void);

/*---------------------------- Module Variables ---------------------------*/
static Timer_t TMR_TimerArray[sizeof(Tflag_t)*BITS_PER_BYTE]=
                                            { 0x0,
                                              0x0,
                                              0x0,
                                              0x0,
                                              0x0,
                                              0x0,
                                              0x0,
                                              0x0 };

// make this one const to get it put into flash, since it will never change
static Tflag_t const TMR_MaskArray[sizeof(Tflag_t)*BITS_PER_BYTE]=
                                            { BIT0HI,
                                              BIT1HI,
                                              BIT2HI,
                                              BIT3HI,
                                              BIT4HI,
                                              BIT5HI,
                                              BIT6HI,
                                              BIT7HI };

static Tflag_t TMR_ActiveFlags;
static uint16_t time;  /* this is used by the default RTI routine */

static pPostFunc const Timer2PostFunc[sizeof(Tflag_t)*BITS_PER_BYTE] = 
                                            { TIMER0_RESP_FUNC,
                                              TIMER1_RESP_FUNC,
                                              TIMER2_RESP_FUNC,
                                              TIMER3_RESP_FUNC,
                                              TIMER4_RESP_FUNC,
                                              TIMER5_RESP_FUNC,
                                              TIMER6_RESP_FUNC,
                                              TIMER7_RESP_FUNC };
  

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     ES_Timer_Init
 Parameters
     unsigned char Rate set to one of the TMR_RATE_XX values to set the
     RTI rate
 Returns
     None.
 Description
     Initializes the timer module by setting up the RTI with the requested
    rate
 Notes
     None.
 Author
     J. Edward Carryer, 02/24/97 14:23
****************************************************************************/
void ES_Timer_Init(TimerRate_t Rate)
{
   RTICTL = (unsigned char)Rate;     /* set RTI Rate */
   CRGFLG = _S12_RTIF;              /* clear any pending int */
   CRGINT |= _S12_RTIE;             /* enable the RTI int */
   EnableInterrupts;                /* make sure that interrupts are enabled*/
}

/****************************************************************************
 Function
     ES_Timer_SetTimer
 Parameters
     unsigned char Num, the number of the timer to set.
     unsigned int NewTime, the new time to set on that timer
 Returns
     ES_Timer_ERR if requested timer does not exist or has no service 
     ES_Timer_OK  otherwise
 Description
     sets the time for a timer, but does not make it active.
 Notes
     None.
 Author
     J. Edward Carryer, 02/24/97 17:11
****************************************************************************/
ES_TimerReturn_t ES_Timer_SetTimer(uint8_t Num, uint16_t NewTime)
{
   /* tried to set a timer that doesn't exist */
   if( (Num >= ARRAY_SIZE(TMR_TimerArray)) ||
   /* tried to set a timer without a service */
       (Timer2PostFunc[Num] == TIMER_UNUSED) ||
       (NewTime == 0) ) /* no time being set */
      return ES_Timer_ERR;  
   TMR_TimerArray[Num] = NewTime;
   return ES_Timer_OK;
}

/****************************************************************************
 Function
     ES_Timer_StartTimer
 Parameters
     unsigned char Num the number of the timer to start
 Returns
     ES_Timer_ERR for error ES_Timer_OK for success
 Description
     simply sets the active flag in TMR_ActiveFlags to (re)start a
     stopped timer.
 Notes
     None.
 Author
     J. Edward Carryer, 02/24/97 14:45
****************************************************************************/
ES_TimerReturn_t ES_Timer_StartTimer(unsigned char Num)
{
   /* tried to set a timer that doesn't exist */
   if( (Num >= ARRAY_SIZE(TMR_TimerArray)) ||
       /* tried to set a timer with no time on it */
       (TMR_TimerArray[Num] == 0) )
      return ES_Timer_ERR;  
   TMR_ActiveFlags |= TMR_MaskArray[Num]; /* set timer as active */
   return ES_Timer_OK;
}

/****************************************************************************
 Function
     ES_Timer_StopTimer
 Parameters
     unsigned char Num the number of the timer to stop.
 Returns
     ES_Timer_ERR for error (timer doesn't exist) ES_Timer_OK for success.
 Description
     simply clears the bit in TMR_ActiveFlags associated with this
     timer. This will cause it to stop counting.
 Notes
     None.
 Author
     J. Edward Carryer, 02/24/97 14:48
****************************************************************************/
ES_TimerReturn_t ES_Timer_StopTimer(unsigned char Num)
{
   if( Num >= ARRAY_SIZE(TMR_TimerArray) )
      return ES_Timer_ERR;  /* tried to set a timer that doesn't exist */
   TMR_ActiveFlags &= ~(TMR_MaskArray[Num]); /* set timer as inactive */
   return ES_Timer_OK;
}

/****************************************************************************
 Function
     ES_Timer_InitTimer
 Parameters
     unsigned char Num, the number of the timer to start
     unsigned int NewTime, the number of ticks to be counted
 Returns
     ES_Timer_ERR if the requested timer does not exist, ES_Timer_OK otherwise.
 Description
     sets the NewTime into the chosen timer and sets the timer actice to 
     begin counting.
 Notes
     None.
 Author
     J. Edward Carryer, 02/24/97 14:51
****************************************************************************/
ES_TimerReturn_t ES_Timer_InitTimer(unsigned char Num, unsigned int NewTime)
{
   /* tried to set a timer that doesn't exist */
   if( (Num >= ARRAY_SIZE(TMR_TimerArray)) ||
   /* tried to set a timer without a service */
       (Timer2PostFunc[Num] == TIMER_UNUSED) ||
       /* tried to set a timer without putting any time on it */
       (NewTime == 0) )
      return ES_Timer_ERR;  
   TMR_TimerArray[Num] = NewTime;
   TMR_ActiveFlags |= TMR_MaskArray[Num]; /* set timer as active */
   return ES_Timer_OK;
}


/****************************************************************************
 Function
     ES_Timer_GetTime
 Parameters
     None.
 Returns
     the current value of the module variable 'time'
 Description
     Provides the ability to grab a snapshot time as an alternative to using
      the library timers. Can be used to determine how long between 2 events.
 Notes
     this functionality is ancient, though this implementation in the library
     is new.
 Author
     J. Edward Carryer, 06/01/04 08:04
****************************************************************************/
uint16_t ES_Timer_GetTime(void)
{
   return (time);
}

/****************************************************************************
 Function
     ES_Timer_RTI_Resp
 Parameters
     None.
 Returns
     None.
 Description
     This is the new RTI response routine to support the timer module.
     It will increment time, to maintain the functionality of the
     GetTime() timer and it will check through the active timers,
     decrementing each active timers count, if the count goes to 0, it
     will post an event to the corresponding SM and clear the active flag to
     prevent further counting.
 Notes
     None.
 Author
     J. Edward Carryer, 02/24/97 15:06
****************************************************************************/
void interrupt 7 ES_Timer_RTI_Resp(void)
{
static Tflag_t NeedsProcessing;
static uint8_t NextTimer2Process;
static ES_Event NewEvent;

   CRGFLG = _S12_RTIF;   /* clear the source of the int */
   ++time;             /* keep the GetTime() timer running */
   if (TMR_ActiveFlags != 0) /* if !=0 , then at least 1 timer is active */
   {
      // start by getting a list of all the active timers
      NeedsProcessing = TMR_ActiveFlags;
      do{
        // find the MSB that is set
        NextTimer2Process = Byte2MSBitNum[NeedsProcessing-1];
        /* decrement that timer, check if timed out */
        if(--TMR_TimerArray[NextTimer2Process] == 0) 
        {
            NewEvent.EventType = ES_TIMEOUT;
            NewEvent.EventParam = NextTimer2Process;
            /* post the timeout event to the right Service */
            Timer2PostFunc[NextTimer2Process](NewEvent); 
            /* and stop counting */
            TMR_ActiveFlags &= ~(TMR_MaskArray[NextTimer2Process]); 
        }
        // mark off the active timer that we just processed
        NeedsProcessing &= BitNum2ClrMask[NextTimer2Process];
      }while(NeedsProcessing != 0);
      
   }
}
/*------------------------------- Footnotes -------------------------------*/
#ifdef TEST

#include <termio.h>
#include <stdio.h>
#include <timerS12.h>


#define TIME_OUT_DELAY 1221
signed char Message[]={0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x0d, 0};

void main(void)
{
   int p;
   puts("\rStarting, should delay for 10 seconds\r");
   EF_Timer_Init(EF_Timer_RATE_8MS);
   EF_Timer_InitTimer(0, TIME_OUT_DELAY); /* TIME_OUT_DELAY  = 10s w/ 8.19mS interval */
   EF_Timer_InitTimer(1, TIME_OUT_DELAY);
   EF_Timer_InitTimer(2, TIME_OUT_DELAY);
   EF_Timer_InitTimer(3, TIME_OUT_DELAY);
   EF_Timer_InitTimer(4, TIME_OUT_DELAY);
   EF_Timer_InitTimer(5, TIME_OUT_DELAY);
   EF_Timer_InitTimer(6, TIME_OUT_DELAY);
   EF_Timer_InitTimer(7, TIME_OUT_DELAY);
   while(EF_Timer_IsTimerExpired(0) != EF_Timer_EXPIRED)
      ;
   puts("Timed Out\r");

   EF_Timer_InitTimer(7, TIME_OUT_DELAY);
   for (p=0;p<10000;p++ ); /* kill some time */
   EF_Timer_StopTimer(7);
   if (EF_Timer_IsTimerActive(7) != EF_Timer_NOT_ACTIVE)
      puts("Timer Stop Failed\r");
   else
      puts("Timer Stop Succeded, restarting timeout\r");
   EF_Timer_StartTimer(7);
   while(EF_Timer_IsTimerExpired(7) != EF_Timer_EXPIRED)
      ;

   puts("Timed Out Again\r");

   DisableInterrupts;
}
#endif
/*------------------------------ End of file ------------------------------*/

