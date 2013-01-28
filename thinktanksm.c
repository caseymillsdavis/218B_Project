/****************************************************************************
 Module
   TopHSMTemplate.c

 Revision
   2.0.1

 Description
   This is a template for the top level Hierarchical state machine

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 02/08/12 01:39 jec      converted from MW_MasterMachine.c
 02/06/12 22:02 jec      converted to Gen 2 Events and Services Framework
 02/13/10 11:54 jec      converted During functions to return Event_t
                         so that they match the template
 02/21/07 17:04 jec      converted to pass Event_t to Start...()
 02/20/07 21:37 jec      converted to use enumerated type for events
 02/21/05 15:03 jec      Began Coding
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ThinkTankSM.h"
#include "DummyEventGenerator.h"
#include "GameControlSM.h"
 
//#include "PositionControlSM.h"
#include <stdio.h>
#include <string.h>
#include "TreadDriveSM.h"
#include "HopperDriveSM.h"
#include "GameConstants.h"
#include "ES_PostList.h"


/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
static ES_Event DuringThinkTank( ES_Event Event);
static char * Command_Text_Array[] = {  "ES_NO_EVENT","ES_ERROR","ES_INIT","ES_NEW_KEY","ES_TIMEOUT", 
                              "ES_ENTRY","ES_ENTRY_HISTORY","ES_EXIT","MASTER_RESET",
                              "BALLS_IN_PLAY","BIN_CHANGED","WALL_MOVED","WALL_CHOSEN",
                              "BIN_CHOSEN","DEPOSIT_COMPLETE","FAVORITE_BIN_THREATENED",
                              "GOAL_ANGLE_ACHIEVED","EPIC_PUSH_FAIL","DEPOSIT_TIME", 
                              "HOLD_COMMAND","DRIVE_COMMAND","ROTATE_COMMAND",
                              "ROTATION_COMPLETE","ARRIVED","POSITION_FOUND","TURN_COMMAND",
                              "GO_COMMAND","WAIT_COMMAND","ACTIVITY_CHANGED", 
                              "WALL_NO_LONGER_THREATENS_HOPPER","WALL_THREATENS_HOPPER", 
                              "HIGH_POSITION_REACHED_RIGHT","HIGH_POSITION_REACHED_LEFT", 
                              "DUMP_POSITION_REACHED","DEPOSIT_COMMAND","UPDATE_CONTROL",
                              "RASTER_COMMAND","CONTROL_RIGHT_WALL_COMMAND", 
                              "CONTROL_LEFT_WALL_COMMAND","DEPOSIT_IN_BIN_ONE_COMMAND", 
				                  "DEPOSIT_IN_BIN_TWO_COMMAND","DEPOSIT_IN_BIN_THREE_COMMAND",
				                  "DEPOSIT_IN_BIN_FOUR_COMMAND", "STAY_PUT_COMMAND", 
				                  "ES_COMMAND_RECEIVED"};
				                  
static char * CurrentCommand[2];	
//static char * OldCommand;			                  


/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, though if the top level state machine
// is just a single state container for orthogonal regions, you could get
// away without it
static ThinkTankState_t CurrentState;
// with the introduction of Gen2, we need a module level Priority var as well
static uint8_t MyPriority;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitMasterSM

 Parameters
     uint8_t : the priorty of this service

 Returns
     boolean, False if error in initialization, True otherwise

 Description
     Saves away the priority,  and starts
     the top level state machine
 Notes

 Author
     J. Edward Carryer, 02/06/12, 22:06
****************************************************************************/
boolean InitThinkTankSM ( uint8_t Priority )
{
  ES_Event ThisEvent;

  MyPriority = Priority;  // save our priority

  ThisEvent.EventType = ES_ENTRY;
  // Start the Master State machine

  StartThinkTankSM( ThisEvent );

  return True;
}

/****************************************************************************
 Function
     PostMasterSM

 Parameters
     ES_Event ThisEvent , the event to post to the queue

 Returns
     boolean False if the post operation failed, True otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:25
****************************************************************************/
boolean PostThinkTankSM( ES_Event ThisEvent )
{
  return ES_PostToService( MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunMasterSM

 Parameters
   ES_Event: the event to process

 Returns
   ES_Event: an event to return

 Description
   the run function for the top level state machine 
 Notes
   uses nested switch/case to implement the machine.
 Author
   J. Edward Carryer, 02/06/12, 22:09
****************************************************************************/
// make recursive call warning into info
#pragma MESSAGE INFORMATION C1855
ES_Event RunThinkTankSM( ES_Event CurrentEvent )
{
#ifdef RUN_TEST_THINKTANK
#else
   // there is only 1 state at this level and it has 3 concurrent regions
   // running the Game Control, Activity Control, & Position Control state machines.
   CurrentEvent = DuringThinkTank(CurrentEvent);
   
   CurrentCommand[1] = CurrentCommand[0];
   CurrentCommand[0] = Command_Text_Array[CurrentEvent.EventType];
   
   if (CurrentEvent.EventType == ES_TIMEOUT)
   {
      if ((QueryHopperDriveSM() != DUMPING)&&(CurrentEvent.EventParam == THINKTANKTIMER))
      {
      ES_Event DummyEvent;
      DummyEvent.EventType = DEPOSIT_COMMAND;
      DummyEvent.EventParam = 1;
      ES_PostList00(DummyEvent);
      printf("dpt tmr end - DUMP\n\r");
      }
      
      //ES_Timer_InitTimer(THINKTANKTIMER, DISPLAY_REFRESH_TIME);
      
      //UpdateDisplay();
   }
   /*
   if ((CurrentEvent.EventType) == EPIC_PUSH_FAIL)
   {
      CheckPointsPlusPlus();
   }
   */

   CurrentEvent.EventType = ES_NO_EVENT;
   return(CurrentEvent);
#endif
}
/****************************************************************************
 Function
     StartMasterSM

 Parameters
     ES_Event CurrentEvent

 Returns
     nothing

 Description
     Does any required initialization for this state machine
 Notes

 Author
     J. Edward Carryer, 02/06/12, 22:15
****************************************************************************/
void StartThinkTankSM ( ES_Event CurrentEvent )
{
  // local variable to get debugger to display the value of CurrentEvent
  volatile ES_Event LocalEvent = CurrentEvent;
  // if there is more than 1 state to the top level machine you will need 
  // to initialize the state variable
  CurrentState = GAMBIT;
  // now we need to let the Run function init the lower level state machines
  // use LocalEvent to keep the compiler from complaining about unused var
  RunThinkTankSM(LocalEvent);
  return;
}


/***************************************************************************
 private functions
 ***************************************************************************/

static ES_Event DuringThinkTank( ES_Event Event )
{
    // process ES_ENTRY & ES_EXIT events
    if ( Event.EventType == ES_ENTRY)
    {
        // implement any entry actions required for this state machine
      // ES_Timer_InitTimer(THINKTANKTIMER, DISPLAY_REFRESH_TIME);
        // after that start any lower level machines that run in this state
        StartGameControlSM(Event);
        //StartActivityControlSM(Event);
        //StartPositionControlSM(Event);
    }else if ( Event.EventType == ES_EXIT)
    {
        // on exit, give the lower levels a chance to clean up first
        RunGameControlSM(Event);
        //RunActivityControlSM(Event);
        //RunPositionControlSM(Event);
    }else
    // do the 'during' function for this state
    {
        // run any lower level state machine
        RunGameControlSM(Event);
        //RunActivityControlSM(Event);
        //RunPositionControlSM(Event);
    }
    return(Event);
}

/*
void UpdateDisplay(void)
{
   //static int DeltaHistory;
   GameState_t CurrentGameControlState = QueryGameControlSM();
   //PositionState_t CurrentPositionControlState = QueryPositionControlSM();
   ActivityState_t CurrentActivityControlState = QueryActivityControlSM();
   TreadState_t CurrentTreadDriveState = QueryTreadDriveSM();
   HopperState_t CurrentHopperDriveState = QueryHopperDriveSM();
   
   printf("Game: ");
   switch (CurrentGameControlState)
   {
      case (FIND_SIDE) :   printf("Find Side  | "); break;
      case (JUST_RASTER) : printf("Just Raster| "); break;
      case (DECIDE_BIN) :  printf("Decide  Bin| "); break;
      case (DECIDE_WALL) : printf("Decide Wall| "); break;
      case (PUSH_WALL) :   printf("Push Wall  | "); break;
      case (DEPOSIT) :     printf("Deposit    | "); break;
   }
   printf("Activity: ");
   switch (CurrentActivityControlState)
   {
      case (STAY_PUT) :             printf("Stay Put  | "); break;
      case (RASTER) :               printf("Raster    | "); break;
      case (CONTROL_LEFT_WALL) :    printf("Ctrl Left | "); break;
      case (CONTROL_RIGHT_WALL) :   printf("Ctrl Right| "); break;
      case (DEPOSIT_IN_BIN_ONE) :   printf("Deposit 1 | "); break;
      case (DEPOSIT_IN_BIN_TWO) :   printf("Deposit 2 | "); break;
      case (DEPOSIT_IN_BIN_THREE) : printf("Deposit 3 | "); break;
      case (DEPOSIT_IN_BIN_FOUR) :  printf("Deposit 4 | "); break;
   }
   
*/   
   /*
   printf("Position: ");
   switch (CurrentPositionControlState)
   {
      case (ASKING_FOR_NEXT_STEP) :         printf("Ask Next  | "); break;
      case (ROTATING_BY_THETA) :            printf("Turn By X | "); break;
      case (DRIVING_A_DISTANCE) :           printf("Drive D   | "); break;
      case (WAITING_FOR_DESIGNATED_EVENT) : printf("Wait For E| "); break;
   }
   */
   
/*   
   printf("Tread: ");
   switch (CurrentTreadDriveState)
   {
      case (ROTATING) : printf("Rotating| "); break;
      case (DRIVING) :  printf("Driving | "); break;
      case (HOLDING) :  printf("Holding | "); break;         
   }
   printf("Hopper: ");
   switch (CurrentHopperDriveState)
   {
      case (HOPPER_ACTIVE) :                      printf("Hopper ON   | "); break;
      case (HOPPER_INACTIVE) :                    printf("Hopper OFF  | "); break;
      case (GOING_TO_HIGH_POSITION_NEITHERHIGH) : printf("Neither High| "); break;
      case (GOING_TO_HIGH_POSITION_RIGHTHIGH) :   printf("Right High  | "); break;
      case (GOING_TO_HIGH_POSITION_LEFTHIGH) :    printf("Left High   | "); break;
      case (DUMPING) :                            printf("Dumping     | "); break;   
   }
   //DeltaHistory=GetDeltaHistory();
   printf("Right Count is %5d, PWM is %d. Left Count is %5d, PWM is %d. Delta is %5d", GetRightCount(), GetPWMRightHopper(), GetLeftCount(), GetPWMLeftHopper(), GetDelta());
   //printf("Current Event (before timer) is: ");
  
   
   //printf(CurrentCommand[1]);
   printf("\r");
} */