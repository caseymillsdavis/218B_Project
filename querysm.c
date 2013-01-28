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
#include "QuerySM.h"
#include "CommandCom.h"
#include <stdio.h>
#include "GameConstants.h"	

/*----------------------------- Module Defines ----------------------------*/
// update with correct timer number


/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
typedef enum { 	initialBalls = 0xFD,
				bin1Balls = 0xFC,
				bin2Balls = 0xFB, 
				bin3Balls = 0xFA,
				bin4Balls = 0xF9,
				wallAngle = 0xF8} FieldStatus_t;
			   
FieldStatus_t CurrentCommand = initialBalls;
static unsigned int FieldStatus[6] = { 0 };  // really should probably be unsigned char
static int i = 0;
static TemplateState_t CurrentState;

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
boolean InitQuerySM ( uint8_t Priority )
{
  ES_Event ThisEvent;

  MyPriority = Priority; 
  // put us into the Initial PseudoState
  CurrentState = InitPState;

  // initialize the command communication module
  CommandComInit();

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
boolean PostQuerySM( ES_Event ThisEvent )
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
ES_Event RunQuerySM( ES_Event ThisEvent )
{
	ES_Event ReturnEvent;
	ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
	
	switch ( CurrentState )
	{
		case InitPState :
			if ( ThisEvent.EventType == ES_INIT )
			{
			  CurrentState = QueryControl;
			  ES_Timer_InitTimer(FSR_TIMER, FSR_COMMAND_TIME); // update with correct timer
			}
			break;

		case QueryControl :
		  switch ( ThisEvent.EventType )
		  {
			  case ES_TIMEOUT :
			  {
			    if ( ThisEvent.EventParam == FSR_TIMER )
				{
					FieldStatusCommand((char)CurrentCommand);
				}
			   }
			   break;
			  
			  case ES_COMMAND_RECEIVED :
			  {
				GetCommand();
				CurrentState = UpdateFieldStatus;
			   }
			   break;
		  }
		  break;
		  
		case UpdateFieldStatus:
		  switch( ThisEvent.EventType )
		  {
			 
			 case ES_COMMAND_RECEIVED :
			 {
			   if ( ThisEvent.EventParam == 0xFF ){
				
				//printf("FSR Offline \n\r");
				
				CurrentState = QueryControl;
				ES_Timer_InitTimer(FSR_TIMER, FSR_COMMAND_TIME); 
				break;
			   } else if ( ThisEvent.EventParam == 0xFE){
			    //printf("Invalid Command Byte Received \n\r");
				CurrentState = QueryControl;
				ES_Timer_InitTimer(FSR_TIMER, FSR_COMMAND_TIME); 
				break;
			   }
			   
			   switch( CurrentCommand )
			   {
				 case initialBalls:
					FieldStatus[i] = ThisEvent.EventParam - 0x80;
			      break;
				 case bin1Balls:
					FieldStatus[i] = ThisEvent.EventParam - 1;
					break;
				 case bin2Balls:
					FieldStatus[i] = ThisEvent.EventParam - 1;
					break;
				 case bin3Balls:
					FieldStatus[i] = ThisEvent.EventParam - 1;
					break;
				 case bin4Balls:
					FieldStatus[i] = ThisEvent.EventParam - 1;
					break;
				 case wallAngle:
					FieldStatus[i] = (ThisEvent.EventParam - 1) * 2;
					break;
				}
				
				//printf("Balls in Play: %d, Bin1: %d, Bin2: %d, Bin3: %d, Bin4: %d, Angle: %d\n\r", FieldStatus[0], FieldStatus[1], FieldStatus[2], FieldStatus[3], FieldStatus[4], FieldStatus[5]);
				
				if (CurrentCommand == wallAngle)
				{
					CurrentCommand = initialBalls;
				} else
				{
					CurrentCommand = CurrentCommand - 1;
				}
				
				i = (i+1) % 6;
				CurrentState = QueryControl;
				ES_Timer_InitTimer(FSR_TIMER, FSR_COMMAND_TIME);        
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
/*TemplateState_t QueryQuerySM ( void )
{
   return(CurrentState);
}*/ 

/***************************************************************************
 access functions
 ***************************************************************************/
unsigned int queryFSR ( int fsrType )
{
   
   return(FieldStatus[fsrType]);
}      
 
 /***************************************************************************
 private functions
 ***************************************************************************/

 