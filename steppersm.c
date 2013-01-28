/****************************************************************************
 Module
   TemplateFSM.c

 Revision
   1.0.1

 Description
   This is a template file for implementing flat state machines under the 
   Gen2 Events and Services Framework.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 01/15/12 11:12 jec      revisions for Gen2 framework
 11/07/11 11:26 jec      made the queue static
 10/30/11 17:59 jec      fixed references to CurrentEvent in RunTemplateSM()
 10/23/11 18:20 jec      began conversion from SMTemplate.c (02/20/07 rev)
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_Timers.h"
#include "ES_PostList.h"
#include "ES_Types.h"
#include "StepperSM.h"
#include <stdio.h>	
#include "S12eVec.h"
#include "ME218_E128.h"
#include "GameConstants.h"

/*----------------------------- Module Defines ----------------------------*/


/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/
void StepperInitialize(void);
//void InitStepperTimer(void);
void SetStepperTimer(void);
void interrupt _Vec_tim0ch7 StepperInterrupt(void);


/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static StepperState_t CurrentState;
static int stepCount;
static boolean pulseFinish = False;
static int currentDirection;
//static unsigned int uLastSpeed = 0;
//static unsigned int uSpeed = 0;

// with the introduction of Gen2, we need a module level Priority var as well
static uint8_t MyPriority;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitTemplateFSM

 Parameters
     uint8_t : the priorty of this service

 Returns
     boolean, False if error in initialization, True otherwise

 Description
     Saves away the priority, sets up the initial transition and does any 
     other required initialization for this state machine
 Notes

 Author
     J. Edward Carryer, 10/23/11, 18:55
****************************************************************************/
boolean InitStepperSM ( uint8_t Priority )
{
  ES_Event ThisEvent;

  MyPriority = Priority; 
  // put us into the Initial PseudoState
  CurrentState = InitPState_Stepper;

  //printf("Begin stepper initialization. \n\r");
  // Initialize stepper pins
  StepperInitialize();
  // Initialize stepper interrupt timer
  //InitStepperTimer(); done elsewhere
  //printf("End stepper initialization. \n\r");

  // post the initial transition event
  ThisEvent.EventType = ES_INIT;
  if (ES_PostToService( MyPriority, ThisEvent) == True)
  {
      return True;
  }else
  {
      return False;
  }
}

/****************************************************************************
 Function
     PostTemplateFSM

 Parameters
     EF_Event ThisEvent , the event to post to the queue

 Returns
     boolean False if the Enqueue operation failed, True otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:25
****************************************************************************/
boolean PostStepperSM( ES_Event ThisEvent )
{
  return ES_PostToService( MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunTemplateFSM

 Parameters
   ES_Event : the event to process

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   add your description here
 Notes
   uses nested switch/case to implement the machine.
 Author
   J. Edward Carryer, 01/15/12, 15:23
****************************************************************************/
ES_Event RunStepperSM( ES_Event ThisEvent )
{
	ES_Event ReturnEvent;
	ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
	
	switch ( CurrentState )
	{
		case InitPState_Stepper :
			if ( ThisEvent.EventType == ES_INIT )
			{
			  CurrentState = HoldingStepper;
			  //printf("StepperSM---->Move to state: HoldingStepper\n\r");
			}
			break;

		case HoldingStepper :
		  switch ( ThisEvent.EventType )
		  {
			  case STEP_CCW :
			  {
			    CurrentState = StepCCW;
				//printf("StepperSM---->Move to state: StepCCW\n\r");
				TIM0_TIE |= _S12_C7I;		//enable OC7 (stepper) interrupt
				TIM0_TCTL1 = TIM0_TCTL1 | _S12_OL7; //toggle pin
				PTT |= DIR_PIN; //update: set direction pin for CCW rotation 
				currentDirection  = -1;
				SetStepperTimer();
				//PTT &= ~STEP_PIN;				//initially set step pin low
			   }
			   break;
			  
			  case STEP_CW :
			  {
				CurrentState = StepCW;
				//printf("StepperSM---->Move to state: StepCW\n\r");
				TIM0_TIE |= _S12_C7I;		//enable OC7 (stepper) interrupt
				TIM0_TCTL1 = TIM0_TCTL1 | _S12_OL7; //toggle pin
				PTT &= ~DIR_PIN; //update: set direction pin for CCW rotation
            currentDirection  = -1;			
            SetStepperTimer();    	
				//PTT &= ~STEP_PIN;				//initially set step pin low
			   }
			   break;
		  }
		  break;
		  
		case StepCCW:
		  switch( ThisEvent.EventType )
		  {
			 
			 /*case TAKE_STEP :
			 {
			    
				PTT ^= STEP_PIN; //Toggle step pin
				
				if (pulseFinish){
					stepCount++;
					pulseFinish = ~pulseFinish;
					//printf("StepperSM---->CCW TAKE_STEP event finished\n\r");
					//printf("StepperSM---->stepCount is:     %d\n\r", stepCount);
				}
				
				pulseFinish = ~pulseFinish;
					
			 }
			 break;*/
			 
			 case STEP_CW :
			  {
				PTT &= ~DIR_PIN; //update: set direction pin for CW rotation
				currentDirection  = -1;
				CurrentState = StepCW;
				//printf("StepperSM---->Move to state: StepCW\n\r");
			   }
			   break;
			 
			 case STOP_STEPPING:
			 {
			    TIM0_TIE = TIM0_TIE & ~_S12_C7I; //disable OC4 (stepper) interrupt
			    TIM0_TCTL1 = TIM0_TCTL1 & ~_S12_OL7;
				CurrentState = HoldingStepper;
				//printf("StepperSM---->Move to state: HoldingStepper\n\r");
				//printf("Last speed recorded: %d\n\r", uSpeed);
			 }
			 break;
			 
			 case ZERO_FOUND:
			 {
				stepCount = 0;
				//printf("StepperSM---->ZERO_FOUND event executed.\n\r");
			 }
			 break;
		  }
		  break;
		  
		  case StepCW:
			switch( ThisEvent.EventType )
			{
			 
				/*case TAKE_STEP :
				{
				
				PTT ^= STEP_PIN;

				if (pulseFinish){
					stepCount--;
					pulseFinish = ~pulseFinish; 
					//printf("StepperSM---->CCW TAKE_STEP event finished\n\r");
					//printf("StepperSM---->stepCount is:     %d\n\r", stepCount);
				}

				pulseFinish = ~pulseFinish;

				}
				break;*/

				case STEP_CCW :
				{
				CurrentState = StepCCW;
				PTT |= DIR_PIN; //update: set direction pin for CCW rotation
				currentDirection  = 1; 
				//printf("StepperSM---->Move to state: StepCCW\n\r");
				}
				break;			 

				case STOP_STEPPING:
				{
				TIM0_TIE = TIM0_TIE & ~_S12_C7I; //disable OC7 (stepper) interrupt
				TIM0_TCTL1 = TIM0_TCTL1 & ~_S12_OL7;
				CurrentState = HoldingStepper;
				//printf("StepperSM---->Move to state: HoldingStepper\n\r");
				//printf("Last speed recorded: %d\n\r", uSpeed);
				}
				break;

				case ZERO_FOUND:
				{
				stepCount = 0;
				printf("StepperSM---->ZERO_FOUND event executed.\n\r");
				}
				break;
			 
			}
		    break;
	} 
	return ReturnEvent;
}

/****************************************************************************
 Function
     QueryTemplateSM

 Parameters
     None

 Returns
     TemplateState_t The current state of the Template state machine

 Description
     returns the current state of the Template state machine
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:21
****************************************************************************/
/*TemplateState_t QueryStepperSM ( void )
{
   return(CurrentState);
}*/ 

/***************************************************************************
 access functions
 ***************************************************************************/

int accessStepCount(void){
	return stepCount; // stepCount is actually 2 times the number of steps taken
}
 
 /***************************************************************************
 private functions
 ***************************************************************************/

void StepperInitialize(void){
	DDRT |= (DIR_PIN | STEP_PIN | MS2_PIN); 		// Make pin 2,3,4 into outputs
	//PTT &= ~MS2_PIN; // Pull this pin low for half stepping, high for microstepping
	PTT |= MS2_PIN;
}

/*void InitStepperTimer(void)
{
	TIM0_TSCR1 = _S12_TEN;							// enable timers
	TIM0_TSCR2 = _S12_PR0 | _S12_PR1 | _S12_PR2;	// prescale to 128
	TIM0_TIOS |= _S12_IOS7;							// set IO line 7 to OC
	TIM0_TCTL1 = TIM0_TCTL1 & ~_S12_OM7;			// toggle pin T3
	TIM0_TCTL1 = TIM0_TCTL1 | _S12_OL7;				// ||
	TIM0_TIE = TIM0_TIE & ~_S12_C7I;				// disable OC7 interrupt
	TIM0_TFLG1 = _S12_C7F;							// clear flag
	TIM0_OC7M = 0x00;
	EnableInterrupts;								// global enable	
}*/

void SetStepperTimer(void)
{
	TIM0_TC7 = TIM0_TCNT + SPEED;		// set timer
}

void interrupt _Vec_tim0ch7 StepperInterrupt(void)
{
	//ES_Event StepperEvent;
	
	TIM0_TC7 += SPEED;					// reset timer
	TIM0_TFLG1 = _S12_C7F;					// clear flag
	
	stepCount = stepCount + currentDirection;
	
	//printf("In the interrupt\r\n");
	
	//uSpeed = TIM0_TC7 - uLastSpeed;
	//uLastSpeed = TIM0_TC7;	
	
    //StepperEvent.EventType = TAKE_STEP;	// move stepper motor forward one step
    //StepperEvent.EventParam = 1;
    //ES_PostList07(StepperEvent);			// update post list; this must be faster than step speed
	
}
	