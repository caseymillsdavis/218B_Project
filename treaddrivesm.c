/****************************************************************************
 Module
   HSMTemplate.c

 Revision
   2.0.1

 Description
   This is a template file for implementing state machines.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 02/08/12 09:56 jec      revisions for the Events and Services Framework Gen2
 02/13/10 14:29 jec      revised Start and run to add new kind of entry function
                         to make implementing history entry cleaner
 02/13/10 12:29 jec      added NewEvent local variable to During function and
                         comments about using either it or Event as the return
 02/11/10 15:54 jec      more revised comments, removing last comment in during
                         function that belongs in the run function
 02/09/10 17:21 jec      updated comments about internal transitions on During funtion
 02/18/09 10:14 jec      removed redundant call to RunLowerlevelSM in EV_Entry
                         processing in During function
 02/20/07 21:37 jec      converted to use enumerated type for events & states
 02/13/05 19:38 jec      added support for self-transitions, reworked
                         to eliminate repeated transition code
 02/11/05 16:54 jec      converted to implment hierarchy explicitly
 02/25/03 10:32 jec      converted to take a passed event parameter
 02/18/99 10:19 jec      built template from MasterMachine.c
 02/14/99 10:34 jec      Began Coding
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
// Basic includes for a program using the Events and Services Framework
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_PostList.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "TreadDriveSM.h"

#include "MotorDrivers.h"
#include "GameConstants.h"
#include "PathPlannerModule.h"

/*----------------------------- Module Defines ----------------------------*/
// define constants for the states for this machine
// and any other local defines

// NOW IN GAMECONSTANTS.  #define ZERO_SPEED 0
#define ROTATION_TIMER 5 // THIS NEEDS TO BE MOVED UP TO GAMECONSTANTS!

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine, things like during
   functions, entry & exit functions.They should be functions relevant to the
   behavior of this state machine
*/
static ES_Event DuringRotating( ES_Event Event);
static ES_Event DuringDriving( ES_Event Event);
static ES_Event DuringHolding( ES_Event Event);

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well
static TreadState_t CurrentState;
static float ExactGoalTime;
static unsigned int GoalTime;
static ES_Event WaitEvent;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
    RunTemplateSM

 Parameters
   ES_Event: the event to process

 Returns
   ES_Event: an event to return

 Description
   add your description here
 Notes
   uses nested switch/case to implement the machine.
 Author
   J. Edward Carryer, 2/11/05, 10:45AM
****************************************************************************/
// make recursive call warning into info
#pragma MESSAGE INFORMATION C1855
ES_Event RunTreadDriveSM( ES_Event CurrentEvent )
{
   unsigned char MakeTransition = False;/* are we making a state transition? */
   TreadState_t NextState = CurrentState;
   ES_Event EntryEventKind = { ES_ENTRY, 0 };// default to normal entry to new state
   ES_Event ReturnEvent = CurrentEvent; // assume we are not consuming event
    
   switch ( CurrentState )
   {
       
       
       
       case ROTATING :       // If current state is state one
         
         //printf("ROTATING\n\r");
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         CurrentEvent = DuringRotating(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
               case TURN_COMMAND : //If event is event one
                  
                  //printf("I think I should turn this far %d\n\r", (signed int) CurrentEvent.EventParam );
                  ExactGoalTime = CONVERT_ANGLE_TO_TIME*(/*(float)*/((signed int)(CurrentEvent.EventParam)));
                  GoalTime = (unsigned int) ((int) fabs(ExactGoalTime));
      
                  if (GoalTime == 0)
                  {
                     GoalTime = 1;
                  }
                  ES_Timer_InitTimer(MOVEMENT_TIMER,GoalTime);
                  
                  Rotate((ExactGoalTime/fabs(ExactGoalTime)));
                  //printf("ExactGoalTime is %f, GoalTime is %d\n\r",ExactGoalTime,GoalTime); 
                  
                  
                  
                  break;
               
               case GO_COMMAND : //If event is event one
                  
                  ExactGoalTime = (CONVERT_DISTANCE_TO_TIME*(/*(float)*/((signed int)(CurrentEvent.EventParam))));
                  //printf("Current Parameter  = %d\n\r",CurrentEvent.EventParam);
                  ExactGoalTime = ExactGoalTime/100;
                  //printf("Exact Goal Time = %f\n\r",ExactGoalTime);
                  GoalTime = (unsigned int) ((int) fabs(ExactGoalTime));
                  //printf("Exact Goal Time = %f\n\r",ExactGoalTime);
                  if (GoalTime == 0)
                  {
                     GoalTime = 1;
                  }
                  ES_Timer_InitTimer(MOVEMENT_TIMER,GoalTime);
                  Drive(100);
                  //printf("ExactGoalTime is %f, GoalTime is %d\n\r",ExactGoalTime,GoalTime);
                  //printf("just started driving at %d duty cycle\n\r", CurrentEvent.EventParam);
                  
                  
                  // Execute action function for state one : event one
                  NextState = DRIVING;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
                // repeat cases as required for relevant events
                case WAIT_COMMAND : //If event is event one
                  
                   
                  
                  switch (CurrentEvent.EventParam)
                  {
                  //ES_Event ActionEvent;   
                     {
                        ES_Event HopperEvent;
                        HopperEvent.EventType = WALL_NO_LONGER_THREATENS_HOPPER;
                        HopperEvent.EventParam = 1;
                        ES_PostList00(HopperEvent);   
                        
                        WaitEvent.EventType = POSITION_FOUND;
                        Drive(ZERO_SPEED);
                        }
                     
                     case WAIT_FOR_DEPOSIT_COMPLETE :
                        WaitEvent.EventType = DEPOSIT_COMPLETE;
                        Drive(ZERO_SPEED);
                        //ActionEvent.EventType = DEPOSIT_COMMAND;
                        //ActionEvent.EventParam = 1;
                        //ES_PostList00(ActionEvent);
                        //ES_PostList01(ActionEvent);
                        
                     break;
                     
                  }
                  
                  // Execute action function for state one : event one
                  NextState = HOLDING;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
                  
                case ES_TIMEOUT : //If event is event one
                  
                  if (CurrentEvent.EventParam == MOVEMENT_TIMER)
                  {
                  ES_Event HopperEvent;
                        HopperEvent.EventType = WALL_THREATENS_HOPPER;
                        HopperEvent.EventParam = 1;
                        ES_PostList00(HopperEvent);
                  
                  
                  Drive(ZERO_SPEED);
                  PathPlanner();
                  
                  NextState = HOLDING;
                  MakeTransition = True;
                  EntryEventKind.EventType = ES_ENTRY;
                  
                  }
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
                  
               case ACTIVITY_CHANGED :
                  PathPlanner();
                  ReturnEvent = CurrentEvent;
               break;
                
            }
         }
         break;
         

         
         case HOLDING :       // If current state is state one
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         //printf("HOLDING\n\r");
         
         CurrentEvent = DuringHolding(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            if (CurrentEvent.EventType == WaitEvent.EventType)
            {                    
               ES_Event HopperEvent;
                        HopperEvent.EventType = WALL_THREATENS_HOPPER;
                        HopperEvent.EventParam = 1;
                        ES_PostList00(HopperEvent);
               PathPlanner(); 

               ReturnEvent = CurrentEvent;   
            } else if (CurrentEvent.EventType == ACTIVITY_CHANGED)
            {                    
               ES_Event HopperEvent;
                        HopperEvent.EventType = WALL_THREATENS_HOPPER;
                        HopperEvent.EventParam = 1;
                        ES_PostList00(HopperEvent);
               PathPlanner();

               ReturnEvent = CurrentEvent;   
            }
            
            switch (CurrentEvent.EventType)
            {
               case TURN_COMMAND : //If event is event one
                   {
                   ES_Event HopperEvent;
                        HopperEvent.EventType = WALL_THREATENS_HOPPER;
                        HopperEvent.EventParam = 1;
                        ES_PostList00(HopperEvent);  
                   
                  //printf("I think I should turn this far %d\n\r", (signed int) CurrentEvent.EventParam );
                  ExactGoalTime = CONVERT_ANGLE_TO_TIME*(/*(float)*/((signed int)(CurrentEvent.EventParam)));
                  GoalTime = (unsigned int) ((int) fabs(ExactGoalTime));
      
                  if (GoalTime == 0)
                  {
                     GoalTime = 1;
                  }
                  ES_Timer_InitTimer(MOVEMENT_TIMER,GoalTime);
                  
                  Rotate((ExactGoalTime/fabs(ExactGoalTime)));
                  //printf("ExactGoalTime is %f, GoalTime is %d\n\r",ExactGoalTime,GoalTime); 
                  
                  // Execute action function for state one : event one
                  NextState = ROTATING;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                   }
                  break;
               
               case GO_COMMAND : //If event is event one
                  {
                  ES_Event HopperEvent;
                        HopperEvent.EventType = WALL_THREATENS_HOPPER;
                        HopperEvent.EventParam = 1;
                        ES_PostList00(HopperEvent);   
                  
                  
                  ExactGoalTime = (CONVERT_DISTANCE_TO_TIME*(/*(float)*/((signed int)(CurrentEvent.EventParam))));
                  //printf("Current Parameter  = %d\n\r",CurrentEvent.EventParam);
                  ExactGoalTime = ExactGoalTime/100;
                  //printf("Exact Goal Time = %f\n\r",ExactGoalTime);
                  GoalTime = (unsigned int) ((int) fabs(ExactGoalTime));
                  //printf("Exact Goal Time = %f\n\r",ExactGoalTime);
                  if (GoalTime == 0)
                  {
                     GoalTime = 1;
                  }
                  ES_Timer_InitTimer(MOVEMENT_TIMER,GoalTime);
                  Drive(100);
                  printf("ExactGoalTime is %f, GoalTime is %d\n\r",ExactGoalTime,GoalTime);
                  //printf("just started driving at %d duty cycle\n\r", CurrentEvent.EventParam);
                  
                  
                  // Execute action function for state one : event one
                  NextState = DRIVING;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  }
                  break;
                // repeat cases as required for relevant events
                case WAIT_COMMAND : //If event is event one
                  
                     
                  
                  
                  
                  switch (CurrentEvent.EventParam)
                  {
                  //ES_Event ActionEvent;   
                     case WAIT_FOR_POSITION_FOUND :
                        {
                        ES_Event HopperEvent;
                        HopperEvent.EventType = WALL_NO_LONGER_THREATENS_HOPPER;
                        HopperEvent.EventParam = 1;
                        ES_PostList00(HopperEvent);   
                        
                        WaitEvent.EventType = POSITION_FOUND;
                        Drive(ZERO_SPEED);
                        }
                     break;
                     
                     case WAIT_FOR_DEPOSIT_COMPLETE :
                        WaitEvent.EventType = DEPOSIT_COMPLETE;
                        Drive(ZERO_SPEED); 
                        
                        //ActionEvent.EventType = DEPOSIT_COMMAND;
                        //ActionEvent.EventParam = 1;
                        //ES_PostList02(ActionEvent);
                     break;
                  }
    
                  
            }
            
            
            
         }
         break;
         
         case DRIVING :       // If current state is state one
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         //printf("DRIVING\n\r");
         CurrentEvent = DuringDriving(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
               case TURN_COMMAND : //If event is event one
                  {
                  ES_Event HopperEvent;
                        HopperEvent.EventType = WALL_THREATENS_HOPPER;
                        HopperEvent.EventParam = 1;
                        ES_PostList00(HopperEvent);   
                  
                  //printf("I think I should turn this far %d\n\r", (signed int) CurrentEvent.EventParam );
                  ExactGoalTime = CONVERT_ANGLE_TO_TIME*(/*(float)*/((signed int)(CurrentEvent.EventParam)));
                  GoalTime = (unsigned int) ((int) fabs(ExactGoalTime));
      
                  if (GoalTime == 0)
                  {
                     GoalTime = 1;
                  }
                  ES_Timer_InitTimer(MOVEMENT_TIMER,GoalTime);
                  Rotate((ExactGoalTime/fabs(ExactGoalTime)));
                  //printf("ExactGoalTime is %f, GoalTime is %d\n\r",ExactGoalTime,GoalTime); 
                  
                  // Execute action function for state one : event one
                  NextState = ROTATING;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  }
                  
                  break;
               
               case GO_COMMAND : //If event is event one
                  {
                  ES_Event HopperEvent;
                        HopperEvent.EventType = WALL_THREATENS_HOPPER;
                        HopperEvent.EventParam = 1;
                        ES_PostList00(HopperEvent);
                     
                  
                  ExactGoalTime = (CONVERT_DISTANCE_TO_TIME*(/*(float)*/((signed int)(CurrentEvent.EventParam))));
                  //printf("Current Parameter  = %d\n\r",CurrentEvent.EventParam);
                  ExactGoalTime = ExactGoalTime/100;
                  //printf("Exact Goal Time = %f\n\r",ExactGoalTime);
                  GoalTime = (unsigned int) ((int) fabs(ExactGoalTime));
                  //printf("Exact Goal Time = %f\n\r",ExactGoalTime);
                  if (GoalTime == 0)
                  {
                     GoalTime = 1;
                  }
                  ES_Timer_InitTimer(MOVEMENT_TIMER,GoalTime);
                  Drive(100);
                  printf("ExactGoalTime is %f, GoalTime is %d\n\r",ExactGoalTime,GoalTime);
                  //printf("just started driving at %d duty cycle\n\r", CurrentEvent.EventParam);
                  
                  
                  
                  }
                  break;
                // repeat cases as required for relevant events
                case WAIT_COMMAND : //If event is event one
                  
                   
                  
                  switch (CurrentEvent.EventParam)
                  {
                  //ES_Event ActionEvent;   
                     case WAIT_FOR_POSITION_FOUND :
                        {
                        ES_Event HopperEvent;
                        HopperEvent.EventType = WALL_NO_LONGER_THREATENS_HOPPER;
                        HopperEvent.EventParam = 1;
                        ES_PostList00(HopperEvent);   
                        
                        WaitEvent.EventType = POSITION_FOUND;
                        Drive(ZERO_SPEED);
                        }
                     break;
                     
                     case WAIT_FOR_DEPOSIT_COMPLETE :
                        WaitEvent.EventType = DEPOSIT_COMPLETE;
                        
                        ES_Timer_InitTimer(DEPOSIT_COMPLETE_TIMER,TIME_NEEDED_TO_COMPLETE_A_DEPOSIT);
                        Drive(ZERO_SPEED);
                        
                        // Move to GameMonitorService when you get a chance
                        //ActionEvent.EventType = DEPOSIT_COMMAND;
                        //ActionEvent.EventParam = 1;
                        //ES_PostList02(ActionEvent);
                     break;
                  }
                  
                  // Execute action function for state one : event one
                  NextState = HOLDING;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
                  
                case ES_TIMEOUT : //If event is event one
                  
                  if (CurrentEvent.EventParam == MOVEMENT_TIMER)
                  {
                  
                  ES_Event HopperEvent;
                        HopperEvent.EventType = WALL_THREATENS_HOPPER;
                        HopperEvent.EventParam = 1;
                        ES_PostList00(HopperEvent);
                  
                  Drive(ZERO_SPEED);
                  PathPlanner();
                  
                  NextState = HOLDING;
                  MakeTransition = True;
                  EntryEventKind.EventType = ES_ENTRY;
                  
                  }
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
                  
                case ACTIVITY_CHANGED :
                  PathPlanner();
                  ReturnEvent = CurrentEvent;
                break;  
                
            }
         }
         break;
      // repeat state pattern as required for other states
    }
    //   If we are making a state transition
    if (MakeTransition == True)
    {
       //   Execute exit function for current state
       CurrentEvent.EventType = ES_EXIT;
       RunTreadDriveSM(CurrentEvent);
        
       CurrentState = NextState; //Modify state variable
       
       //   Execute entry function for new state
       // this defaults to ES_ENTRY
       RunTreadDriveSM(EntryEventKind);
     }
     return(CurrentEvent);
}
/****************************************************************************
 Function
     StartTemplateSM

 Parameters
     None

 Returns
     None

 Description
     Does any required initialization for this state machine
 Notes

 Author
     J. Edward Carryer, 2/18/99, 10:38AM
****************************************************************************/
void StartTreadDriveSM ( ES_Event CurrentEvent )
{
   // local variable to get debugger to display the value of CurrentEvent
   ES_Event LocalEvent = CurrentEvent;
   // to implement entry to a history state or directly to a substate
   // you can modify the initialization of the CurrentState variable
   // otherwise just start in the entry state every time the state machine
   // is started
   if ( ES_ENTRY_HISTORY != CurrentEvent.EventType )
   {
        CurrentState = HOLDING;
        //WaitEvent.EventType = WAIT_FOR_POSITION_FOUND;
   }
   // call the entry function (if any) for the ENTRY_STATE
   RunTreadDriveSM(CurrentEvent);
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
     J. Edward Carryer, 2/11/05, 10:38AM
****************************************************************************/
TreadState_t QueryTreadDriveSM ( void )
{
   return(CurrentState);
}

/***************************************************************************
 private functions
 ***************************************************************************/

static ES_Event DuringRotating( ES_Event Event)
{
    ES_Event ReturnEvent = Event; // assmes no re-mapping or comsumption

    // process EV_ENTRY, EV_ENTRY_HISTORY & EV_EXIT events
    if ( (Event.EventType == ES_ENTRY) || 
         (Event.EventType == ES_ENTRY_HISTORY) )
    {
        // implement any entry actions required for this state machine
        
        
        
        // after that start any lower level machines that run in this state
    //    StartLowerLevelSM( Event );
        // repeat the StartxxxSM() functions for concurrent state machines
        // on the lower level
    }
    else if ( Event.EventType == ES_EXIT )
    {
        // on exit, give the lower levels a chance to clean up first
    //    RunLowerLevelSM(Event);
        // repeat for any concurrently running state machines
        // now do any local exit functionality
    }else
    // do the 'during' function for this state
    {

        

        
        // run any lower level state machine
    //    ReturnEvent = RunLowerLevelSM(Event);
        // repeat for any concurrent lower level machines
        // do any activity that is repeated as long as we are in this state
    }
    // return either Event, if you don't want to allow the lower level machine
    // to remap the current event, or ReturnEvent if you do want to allow it.
    return(ReturnEvent);
}

static ES_Event DuringHolding( ES_Event Event)
{
    ES_Event ReturnEvent = Event; // assmes no re-mapping or comsumption

    // process EV_ENTRY, EV_ENTRY_HISTORY & EV_EXIT events
    if ( (Event.EventType == ES_ENTRY) || 
         (Event.EventType == ES_ENTRY_HISTORY) )
    {
        // implement any entry actions required for this state machine
        // after that start any lower level machines that run in this state
    //    StartLowerLevelSM( Event );
        // repeat the StartxxxSM() functions for concurrent state machines
        // on the lower level
    }
    else if ( Event.EventType == ES_EXIT )
    {
        // on exit, give the lower levels a chance to clean up first
    //    RunLowerLevelSM(Event);
        // repeat for any concurrently running state machines
        // now do any local exit functionality
    }else
    // do the 'during' function for this state
    {
        // run any lower level state machine
    //    ReturnEvent = RunLowerLevelSM(Event);
        // repeat for any concurrent lower level machines
        // do any activity that is repeated as long as we are in this state
    }
    // return either Event, if you don't want to allow the lower level machine
    // to remap the current event, or ReturnEvent if you do want to allow it.
    return(ReturnEvent);
}

static ES_Event DuringDriving( ES_Event Event)
{
    ES_Event ReturnEvent = Event; // assmes no re-mapping or comsumption

    // process EV_ENTRY, EV_ENTRY_HISTORY & EV_EXIT events
    if ( (Event.EventType == ES_ENTRY) || 
         (Event.EventType == ES_ENTRY_HISTORY) )
    {
        // implement any entry actions required for this state machine
        // after that start any lower level machines that run in this state
    //    StartLowerLevelSM( Event );
        // repeat the StartxxxSM() functions for concurrent state machines
        // on the lower level
    }
    else if ( Event.EventType == ES_EXIT )
    {
        // on exit, give the lower levels a chance to clean up first
    //    RunLowerLevelSM(Event);
        // repeat for any concurrently running state machines
        // now do any local exit functionality
    }else
    // do the 'during' function for this state
    {
        // run any lower level state machine
    //    ReturnEvent = RunLowerLevelSM(Event);
    

        

        // repeat for any concurrent lower level machines
        // do any activity that is repeated as long as we are in this state
    }
    // return either Event, if you don't want to allow the lower level machine
    // to remap the current event, or ReturnEvent if you do want to allow it.
    return(ReturnEvent);
}

