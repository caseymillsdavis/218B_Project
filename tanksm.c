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
#include "TankSM.h"
#include "ES_Timers.h"
#include <stdio.h>
#include "PlayingSM.h"
#include "GameConstants.h"
#include "MotorDrivers.h"
#include "ES_PostList.h"
#include "HopperDriveSM.h"
#include "EventCheckers.h"


/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
static ES_Event DuringWaitingForStart( ES_Event Event);
static ES_Event DuringPlaying( ES_Event Event);
static ES_Event DuringHolding( ES_Event Event);
static ES_Event DuringCalibration( ES_Event Event);

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, though if the top level state machine
// is just a single state container for orthogonal regions, you could get
// away without it
static TankState_t CurrentState;
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
boolean InitTankSM ( uint8_t Priority )
{
  ES_Event ThisEvent;

  MyPriority = Priority;  // save our priority

  ThisEvent.EventType = ES_ENTRY;
  // Start the Master State machine

  StartTankSM( ThisEvent );

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
boolean PostTankSM( ES_Event ThisEvent )
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
ES_Event RunTankSM( ES_Event CurrentEvent )
{

   unsigned char MakeTransition = False;/* are we making a state transition? */
   TankState_t NextState = CurrentState;
   ES_Event EntryEventKind = { ES_ENTRY, 0 };// default to normal entry to new state
   
   switch( CurrentState )
   {
      case CALIBRATING_HOPPER :
         // During function time, gives low-level SMs priority in dealing w/event
         CurrentEvent = DuringCalibration(CurrentEvent);
  

         // we only process 1 event in this state, the "Balls in Play" flag to start the game
         if ( CurrentEvent.EventType == CALIBRATION_COMPLETE)
         {
              
              NextState = WAITING_FOR_START;//Decide what the next state will be
              // for internal transitions, skip changing MakeTransition
              MakeTransition = True; //mark that we are taking a transition
              // if transitioning to a state with history change kind of entry
              EntryEventKind.EventType = ES_ENTRY; 
              RIGHT_OR_FRONT_HOPPER_PWM=0;
              LEFT_OR_REAR_HOPPER_PWM=0;
              InitializeCheckers();
               
         }
         break;
      
      
      case WAITING_FOR_START :
         // During function time, gives low-level SMs priority in dealing w/event
         CurrentEvent = DuringWaitingForStart(CurrentEvent);
  

         // we only process 1 event in this state, the "Balls in Play" flag to start the game
         if ( CurrentEvent.EventType == BALLS_IN_PLAY)
         {
              // Execute action function for state one : event one
              #ifndef RUN_HOPPER_CALIBRATION_ROUTINE
              
              ES_Event TriangulateEvent;
              TriangulateEvent.EventType = FIND_POSITION;
              TriangulateEvent.EventParam = 1;
              ES_PostList05(TriangulateEvent);
              
              ES_Timer_InitTimer(MAIN_GAME_TIMER, TWO_MINUTES);
              puts("Whelp, we started that timer.\n\r");
              
              #endif
              
              
              
              NextState = PLAYING;//Decide what the next state will be
              // for internal transitions, skip changing MakeTransition
              MakeTransition = True; //mark that we are taking a transition
              // if transitioning to a state with history change kind of entry
              EntryEventKind.EventType = ES_ENTRY;            
         }
         break;
      
      case PLAYING :
         // During function time, gives low-level SMs priority in dealing w/event
         CurrentEvent = DuringPlaying(CurrentEvent);
  

         // we only process 1 event in this state, the "Timer Out" flag to end the game
         switch ( CurrentEvent.EventType )
         {
             
            case ES_TIMEOUT :
                // See if it's the game timer
                if ( CurrentEvent.EventParam == MAIN_GAME_TIMER )
                {
              puts("And, it expired.  Sweet.\n\r");                             
              NextState = DISABLED;//Decide what the next state will be
              Drive(ZERO_SPEED);
              LEFT_OR_REAR_HOPPER_PWM = 0;
              RIGHT_OR_FRONT_HOPPER_PWM = 0;
              PTU &= ~BLUE_LED;
              PTU &= ~RED_LED;
              TIM0_TIE = TIM0_TIE & ~_S12_C7I; //disable OC4 (stepper) interrupt
			     TIM0_TCTL1 = TIM0_TCTL1 & ~_S12_OL7;
              
                // for internal transitions, skip changing MakeTransition
              MakeTransition = True; //mark that we are taking a transition
                // if transitioning to a state with history change kind of entry
                EntryEventKind.EventType = ES_ENTRY;
                }
            break;
            
            case MASTER_RESET :
                // See if it's the game timer
                if ( CurrentEvent.EventParam == MAIN_GAME_TIMER )
                {
                puts("And, it expired.  Sweet.\n\r");                             
                NextState = DISABLED;//Decide what the next state will be
                // for internal transitions, skip changing MakeTransition
                MakeTransition = True; //mark that we are taking a transition
                // if transitioning to a state with history change kind of entry
                EntryEventKind.EventType = ES_ENTRY;
                }
            break;
         
         }
         break;
         
      case DISABLED :
         // During function time, gives low-level SMs priority in dealing w/event
         CurrentEvent = DuringHolding(CurrentEvent);
  

         // we only process 1 event in this state, the "Master Reset" flag which will probably never happen
         if ( CurrentEvent.EventType == MASTER_RESET)
         {
              puts("Whoa!  Who pushed Master Reset?  And how did you do it?.\n\r"); 
              NextState = WAITING_FOR_START;//Decide what the next state will be
              // for internal transitions, skip changing MakeTransition
              MakeTransition = True; //mark that we are taking a transition
              // if transitioning to a state with history change kind of entry
              EntryEventKind.EventType = ES_ENTRY;
         }
         break;
         
         
   }
   //   If we are making a state transition
   if (MakeTransition == True)
   {
      //   Execute exit function for current state
      CurrentEvent.EventType = ES_EXIT;
      RunTankSM(CurrentEvent);
        
      CurrentState = NextState; //Modify state variable
       
      //   Execute entry function for new state
      // this defaults to ES_ENTRY
      RunTankSM(EntryEventKind);
   }   
   // in the absence of an error the top level state machine should
   // always return ES_NO_EVENT
   CurrentEvent.EventType = ES_NO_EVENT;
   return(CurrentEvent);
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
void StartTankSM ( ES_Event CurrentEvent )
{
  // local variable to get debugger to display the value of CurrentEvent
  volatile ES_Event LocalEvent = CurrentEvent;
  // if there is more than 1 state to the top level machine you will need 
  // to initialize the state variable
  CurrentState = CALIBRATING_HOPPER;
  // now we need to let the Run function init the lower level state machines
  // use LocalEvent to keep the compiler from complaining about unused var
  RunTankSM(LocalEvent);
  return;
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
TankState_t QueryTankSM ( void )
{
   return(CurrentState);
}


/***************************************************************************
 private functions
 ***************************************************************************/

static ES_Event DuringWaitingForStart( ES_Event Event )
{
    // process ES_ENTRY & ES_EXIT events
    if ( Event.EventType == ES_ENTRY)
    {
        // implement any entry actions required for this state machine
        // after that start any lower level machines that run in this state
        
    //    StartMagControlSM(Event);
    }else if ( Event.EventType == ES_EXIT)
    {
        // on exit, give the lower levels a chance to clean up first
        //RunPlayingSM(Event);
        // repeat for any concurrently running state machines
    //    RunMagControlSM(Event);
    }else
    // do the 'during' function for this state
    {
        // run any lower level state machine
        
        // repeat for any concurrent lower level machines
    //    RunMagControlSM(Event);
    }
    return(Event);
}

static ES_Event DuringPlaying( ES_Event Event )
{
    // process ES_ENTRY & ES_EXIT events
    if ( Event.EventType == ES_ENTRY)
    {
        // implement any entry actions required for this state machine
        // after that start any lower level machines that run in this state
         StartPlayingSM(Event);
    //    StartTimerSM(Event);
    //    StartMagControlSM(Event);
    }else if ( Event.EventType == ES_EXIT)
    {
        // on exit, give the lower levels a chance to clean up first
         RunPlayingSM(Event);
        // repeat for any concurrently running state machines
    //    RunMagControlSM(Event);
    }else
    // do the 'during' function for this state
    {
        // run any lower level state machine
        //printf("Event1: %d /n/r", Event.EventType);
         RunPlayingSM(Event);
        // repeat for any concurrent lower level machines
    //    RunMagControlSM(Event);
    }
    return(Event);
}

static ES_Event DuringHolding( ES_Event Event )
{
    // process ES_ENTRY & ES_EXIT events
    if ( Event.EventType == ES_ENTRY)
    {
        // implement any entry actions required for this state machine
        // after that start any lower level machines that run in this state
    //    StartTimerSM(Event);
    //    StartMagControlSM(Event);
    }else if ( Event.EventType == ES_EXIT)
    {
        // on exit, give the lower levels a chance to clean up first
    //    RunTimerSM(Event);
        // repeat for any concurrently running state machines
    //    RunMagControlSM(Event);
    }else
    // do the 'during' function for this state
    {
        // run any lower level state machine
    //    RunTimerSM(Event);
        // repeat for any concurrent lower level machines
    //    RunMagControlSM(Event);
    }
    return(Event);
}

static ES_Event DuringCalibration( ES_Event Event )
{
    // process ES_ENTRY & ES_EXIT events
    if ( Event.EventType == ES_ENTRY)
    {
        // implement any entry actions required for this state machine
        // after that start any lower level machines that run in this state
      StartHopperDriveSM( Event);
    //    StartMagControlSM(Event);
    }else if ( Event.EventType == ES_EXIT)
    {
        // on exit, give the lower levels a chance to clean up first
      RunHopperDriveSM( Event);
        // repeat for any concurrently running state machines
    //    RunMagControlSM(Event);
      
    }else
    // do the 'during' function for this state
    {
        // run any lower level state machine
      RunHopperDriveSM( Event);
        // repeat for any concurrent lower level machines
    //    RunMagControlSM(Event);
    }
    return(Event);
}

