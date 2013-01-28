/****************************************************************************
 Module
   MW_MapKeys.c

 Revision
   1.0.1

 Description
   This service maps keystrokes to events for the microwave oven.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 02/07/12 00:00 jec      converted to service for use with E&S Gen2
 02/20/07 21:37 jec      converted to use enumerated type for events
 02/21/05 15:38 jec      Began coding
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
#include <stdio.h>
#include <ctype.h>
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "MW_MapKeys.h"


/*----------------------------- Module Defines ----------------------------*/
#define CLOSED 1
#define OPEN   0
#define DISPLAY_TIMER 6


// at the usual 32ms/tick this is about a 3Hz update rate
#define DISPLAY_UPDATE_TIME 10

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/
static void UpdateDisplay(void);

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

static unsigned char DoorState = CLOSED;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitMapKeys

 Parameters
     uint8_t : the priorty of this service

 Returns
     boolean, False if error in initialization, True otherwise

 Description
     Saves away the priority, and does any 
     other required initialization for this service
 Notes

 Author
     J. Edward Carryer, 02/07/12, 00:04
****************************************************************************/
boolean InitMapKeys ( uint8_t Priority )
{
  MyPriority = Priority;
  // start up the display timer
  ES_Timer_InitTimer(DISPLAY_TIMER, DISPLAY_UPDATE_TIME);

  return True;
}

/****************************************************************************
 Function
     PostMapKeys

 Parameters
     EF_Event ThisEvent ,the event to post to the queue

 Returns
     boolean False if the Enqueue operation failed, True otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:25
****************************************************************************/
boolean PostMapKeys( ES_Event ThisEvent )
{
  return ES_PostToService( MyPriority, ThisEvent);
}


/****************************************************************************
 Function
    RunMapKeys

 Parameters
   ES_Event : the event to process

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   maps keys to Events for HierMuWave Example
 Notes
   
 Author
   J. Edward Carryer, 02/07/12, 00:08
****************************************************************************/
ES_Event RunMapKeys( ES_Event ThisEvent )
{
  ES_Event ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

    if ( ThisEvent.EventType == ES_NEW_KEY) // there was a key pressed
    {
        switch ( toupper(ThisEvent.EventParam))
        {
            case 'T' : ThisEvent.EventType = DEPOSIT_TIME; 
            case 'F' : ThisEvent.EventType = FAVORITE_BIN_THREATENED; break;
            case 'B' : ThisEvent.EventType = BIN_CHOSEN; 
                       ThisEvent.EventParam = 1; break;
            case 'D' : ThisEvent.EventType = DEPOSIT_COMPLETE; break;
            case 'W' : ThisEvent.EventType = WALL_CHOSEN;
                       ThisEvent.EventParam = 2; break;
            case 'E' : ThisEvent.EventType = EPIC_PUSH_FAIL; break;
            case 'G' : ThisEvent.EventType = GOAL_ANGLE_ACHIEVED; break;
            case 'H' : ThisEvent.EventType = WALL_THREATENS_HOPPER; break; 
            case 'N' : ThisEvent.EventType = WALL_NO_LONGER_THREATENS_HOPPER; break;
            case 'R' : ThisEvent.EventType = HIGH_POSITION_REACHED; break;
            case 'A' : ThisEvent.EventType = ARRIVED; break;
            case 'Q' : ThisEvent.EventType = ROTATION_COMPLETE; break; 
            case 'X' : ThisEvent.EventType = POSITION_FOUND; break;                                                
            
            
            
            
            
            
        }
        /*
        if (ThisEvent.EventType != ES_NEW_KEY) // don't post event for 'D'
          PostMasterSM(ThisEvent);
        */
    }
    
    if ( ThisEvent.EventType == ES_TIMEOUT) // time to update display
    {
      //UpdateDisplay();
      ES_Timer_InitTimer(DISPLAY_TIMER, DISPLAY_UPDATE_TIME);
    }

  return ReturnEvent;
}


/****************************************************************************
 Function
    ChkDoorClosed

 Parameters
   None

 Returns
   boolean, True if door is closed False otherwise

 Description
   provides access to module level door state variable
 Notes
   
 Author
   J. Edward Carryer, 02/07/12, 00:29
****************************************************************************/
static void UpdateDisplay(void)
{
   //int StateNum = 2;
   //printf("We're in state %d\r",2);
}
