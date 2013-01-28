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
#include "ES_General.h"
#include "ES_Events.h"
#include "ES_PostList.h"
#include "ES_ServiceHeaders.h"
#include "ME218_E128.h"
#include "PathPlannerModule.h"

// for the position acess functions
#include "IRSM.h"
             
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "GameControlSM.h"
#include "GameConstants.h"

/*----------------------------- Module Defines ----------------------------*/
// define constants for the states for this machine
// and any other local defines


/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine, things like during
   functions, entry & exit functions.They should be functions relevant to the
   behavior of this state machine
*/
void RedOrBlueSide(void);

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well
static GameState_t CurrentState;
static ActivityState_t CurrentActivity;
static boolean Red;

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
ES_Event RunGameControlSM( ES_Event CurrentEvent )
{
   unsigned char MakeTransition = False;/* are we making a state transition? */
   GameState_t NextState = CurrentState;
   //ActivityState_t NextActivity = CurrentActivity;
   ES_Event EntryEventKind = { ES_ENTRY, 0 };// default to normal entry to new state
   ES_Event ReturnEvent = CurrentEvent; // assume we are not consuming event

   switch ( CurrentState )
   {
       case FIND_SIDE :       // If current state is state one
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         //CurrentEvent = DuringSeekGame(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
               case POSITION_FOUND : //If event is event one
                  {
                   
                  ES_Event TankEvent;
                  float _X_ = GetX();
                  float _Y_ = GetY();
                  float _Q_ = GetTheta();
                  int WallQ = GetWallAngle();
                  
                  
                  ES_Timer_InitTimer(DEPOSIT_TIME_TIMER, 5*ONE_SECOND);

                  
                  RedOrBlueSide();
                  
                  //if (CurrentLocationIsSafe(_X_, _Y_, _Q_, WallQ)==True)
                  //{
                  TankEvent.EventType = WALL_NO_LONGER_THREATENS_HOPPER;
                  TankEvent.EventParam = 1; 
                  ES_PostList00(TankEvent);   
                  //}
                     
                                   
                
                  
                // repeat cases as required for relevant events                              
                  }
                  
               case ES_TIMEOUT : //Move out of switch to bottom of function
                  
                  if(CurrentEvent.EventParam == DEPOSIT_TIME_TIMER)
                  {
                     //ES_Event ActivityEvent;
                     ES_Event HopperEvent;
                     ES_Event TransitionEvent;
                     
                     HopperEvent.EventType = WALL_THREATENS_HOPPER;
                     HopperEvent.EventParam = 1;
                     //ES_PostList00(HopperEvent);
                     //printf("I told it to stop.\n\r");
                     
                     ES_Timer_InitTimer(DEPOSIT_TIME_TIMER, (2*THIRTY_SECONDS));
                     
                     
                     TransitionEvent.EventType = ACTIVITY_CHANGED;
                     TransitionEvent.EventParam = 1;
                     ES_PostList00(TransitionEvent);
                     //NextActivity = RASTER;
                     CurrentActivity = RASTER;
                     
                     
                     // Execute action function for state one : event one
                     NextState = JUST_RASTER;//Decide what the next state will be
                     // for internal transitions, skip changing MakeTransition
                     MakeTransition = True; //mark that we are taking a transition
                     // if transitioning to a state with history change kind of entry
                     EntryEventKind.EventType = ES_ENTRY;
                     // optionally, consume or re-map this event for the upper
                     // level state machine
                     ReturnEvent = CurrentEvent;
                     break; 
                  }
                  
                
            }
         }
         break;
       
       
       case JUST_RASTER :       // If current state is state one
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         //CurrentEvent = DuringSeekGame(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
            ES_Event TransitionEvent;
            //ES_Event ActivityEvent;
               case FAVORITE_BIN_THREATENED : //If event is event one
                  
                  TransitionEvent.EventType = ACTIVITY_CHANGED;
                  TransitionEvent.EventParam = 1;
                  ES_PostList00(TransitionEvent);
                  CurrentActivity = STAY_PUT;
                  NextState = DECIDE_WALL;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
                // repeat cases as required for relevant events
               
               case DEPOSIT_TIME : //If event is event one
                  // Execute action function for state one : event one
                  //ActivityEvent.EventType = STAY_PUT_COMMAND;
                  //ActivityEvent.EventParam = 1;
                  //ES_PostList01(ActivityEvent);
                  
                  TransitionEvent.EventType = ACTIVITY_CHANGED;
                  TransitionEvent.EventParam = 1;
                  ES_PostList00(TransitionEvent);
                  
                  //NextActivity = STAY_PUT;
                  CurrentActivity = STAY_PUT;
                  NextState = DECIDE_BIN;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
               
               
               case ES_TIMEOUT : //Move out of switch to bottom of function
                  
                  if(CurrentEvent.EventParam == DEPOSIT_TIME_TIMER)
                  {
                  
                        ES_Event TransitionEvent;
                        TransitionEvent.EventType = ACTIVITY_CHANGED;
                        TransitionEvent.EventParam = 1;
                        ES_PostList00(TransitionEvent);
                        //NextActivity = DEPOSIT_IN_BIN_ONE;
                        
                        if (Red == True)
                        {
                        CurrentActivity = DEPOSIT_IN_BIN_ONE;   
                        } else 
                        {
                        CurrentActivity = DEPOSIT_IN_BIN_TWO;
                        }
                        ES_Timer_InitTimer(THINKTANKTIMER, THIRTY_SECONDS);
                        //printf("strt dpt tmr\n\r"); 
                        
                        // Execute action function for state one : event one
                        NextState = DEPOSIT;//Decide what the next state will be
                        // for internal transitions, skip changing MakeTransition
                        MakeTransition = True; //mark that we are taking a transition
                        // if transitioning to a state with history change kind of entry
                        EntryEventKind.EventType = ES_ENTRY;
                        // optionally, consume or re-map this event for the upper
                        // level state machine
                        ReturnEvent = CurrentEvent;
                    
                  
                  }
                  
                  break;
                
            }
         }
         break;
      // repeat state pattern as required for other states
      
         case DECIDE_WALL :       // If current state is state one
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         //CurrentEvent = DuringSeekGame(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
               case WALL_CHOSEN : //If event is event one
                  {
                     
                  ES_Event TransitionEvent;
                  
                  
    
                  if (CurrentEvent.EventParam == LEFT)
                  {
                     
                     TransitionEvent.EventType = ACTIVITY_CHANGED;
                     TransitionEvent.EventParam = 1;
                     ES_PostList00(TransitionEvent); 
                     //NextActivity = CONTROL_LEFT_WALL;
                     CurrentActivity = CONTROL_LEFT_WALL;  
                  } else
                  {
                     
                     TransitionEvent.EventType = ACTIVITY_CHANGED;
                     TransitionEvent.EventParam = 1;
                     ES_PostList00(TransitionEvent);
                     //NextActivity = CONTROL_RIGHT_WALL;
                     CurrentActivity = CONTROL_RIGHT_WALL;
                      
                  }
                  // Execute action function for state one : event one
                  NextState = PUSH_WALL;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
                // repeat cases as required for relevant events                              
                  }
                
            }
         }
         break;
         
         
         case DECIDE_BIN :       // If current state is state one
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         //CurrentEvent = DuringSeekGame(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
               case BIN_CHOSEN : //If event is event one
                  {
                     
                   ES_Event TransitionEvent;
                  
                  //ES_Event ThisEvent;
    
                  switch (CurrentEvent.EventParam)
                  {
                     case (1) :
                        
                        TransitionEvent.EventType = ACTIVITY_CHANGED;
                        TransitionEvent.EventParam = 1;
                        ES_PostList00(TransitionEvent);
                        //NextActivity = DEPOSIT_IN_BIN_ONE;
                        CurrentActivity = DEPOSIT_IN_BIN_ONE;  
                     break;
                     
                     case (2) :
                        TransitionEvent.EventType = ACTIVITY_CHANGED;
                        TransitionEvent.EventParam = 1;
                        ES_PostList00(TransitionEvent);
                        //NextActivity = DEPOSIT_IN_BIN_TWO;
                        CurrentActivity = DEPOSIT_IN_BIN_TWO;
                     break;
                     
                     case (3) :
                        TransitionEvent.EventType = ACTIVITY_CHANGED;
                        TransitionEvent.EventParam = 1;
                        ES_PostList00(TransitionEvent);
                        //NextActivity = DEPOSIT_IN_BIN_THREE;
                        CurrentActivity = DEPOSIT_IN_BIN_THREE;
                     break;
                     
                     case (4) :
                        TransitionEvent.EventType = ACTIVITY_CHANGED;
                        TransitionEvent.EventParam = 1;
                        ES_PostList00(TransitionEvent);
                        //NextActivity = DEPOSIT_IN_BIN_FOUR;
                        CurrentActivity = DEPOSIT_IN_BIN_FOUR;
                     break;
                  }
                  // Execute action function for state one : event one
                  NextState = DEPOSIT;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
                // repeat cases as required for relevant events                              
                  }
            }
         }
         break;
      
         case DEPOSIT :       // If current state is state one
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         //CurrentEvent = DuringSeekGame(CurrentEvent);
         //process any events
         /*
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
               ES_Event ThisEvent;
               
               case ES_TIMEOUT : //If event is event one
                  if (CurrentEvent.EventParam == DEPOSIT_COMPLETE_TIMER)
                  {                 
                  ES_Event TransitionEvent;
                     TransitionEvent.EventType = ACTIVITY_CHANGED;
                     TransitionEvent.EventParam = 1;
                     ES_PostList00(TransitionEvent);
                  ActivityState = STAY_PUT;
                  
                  NextState = STAY_PUT;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  }

                  
                // repeat cases as required for relevant events
            }
         } */
         break;
		 
		 case PUSH_WALL :       // If current state is state one
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         //CurrentEvent = DuringSeekGame(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
               ES_Event TransitionEvent;
               
               case EPIC_PUSH_FAIL : //If event is event one
                  // Execute action function for state one : event one
                  NextState = DECIDE_WALL;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
                // repeat cases as required for relevant events

               case GOAL_ANGLE_ACHIEVED : //If event is event one
                  // Execute action function for state one : event one
                  
                     TransitionEvent.EventType = ACTIVITY_CHANGED;
                     TransitionEvent.EventParam = 1;
                     ES_PostList00(TransitionEvent);
                  //NextActivity = RASTER;
                  CurrentActivity = RASTER;
                  NextState = JUST_RASTER;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
                // repeat cases as required for relevant events
                
            }
         }
         break;
      
      
    }
    
    /*
    if (CurrentActivity != NextActivity)
    {
      ES_Event TransitionEvent;
      TransitionEvent.EventType = ACTIVITY_CHANGED;
      TransitionEvent.EventParam = 1;
      ES_PostList00(TransitionEvent);
      CurrentActivity = NextActivity;  
    }
    */
    
    //   If we are making a state transition
    if (MakeTransition == True)
    {
       //   Execute exit function for current state
       CurrentEvent.EventType = ES_EXIT;
       RunGameControlSM(CurrentEvent);
        
       CurrentState = NextState; //Modify state variable
       
       //   Execute entry function for new state
       // this defaults to ES_ENTRY
       RunGameControlSM(EntryEventKind);
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
void StartGameControlSM ( ES_Event CurrentEvent )
{
   // local variable to get debugger to display the value of CurrentEvent
   ES_Event LocalEvent = CurrentEvent;
   // to implement entry to a history state or directly to a substate
   // you can modify the initialization of the CurrentState variable
   // otherwise just start in the entry state every time the state machine
   // is started
   if ( ES_ENTRY_HISTORY != CurrentEvent.EventType )
   {
        //ES_Event ThisEvent;
        //ThisEvent.EventType = STAY_PUT_COMMAND;
        //ThisEvent.EventParam = 0;
        //ES_PostList01(ThisEvent);
        CurrentActivity = STAY_PUT;
        CurrentState = FIND_SIDE;
   }
   // call the entry function (if any) for the ENTRY_STATE
   RunGameControlSM(CurrentEvent);
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
GameState_t QueryGameControlSM ( void )
{
   return(CurrentState);
}

/****************************************************************************
 FunctionACTIVITY

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
ActivityState_t QueryActivityControlSM ( void )
{
   return(CurrentActivity);
}



/***************************************************************************
 private functions
 ***************************************************************************/

void RedOrBlueSide(void)
{
   float currentY = accessYPosition();
   
   if (currentY > (LENGTH/2))
   {
       PTU |= RED_LED;
       PTU &= ~BLUE_LED;
       Red = True;
       //printf("GO ON RED: %f,\n\r", currentY);
   }else if (currentY < (LENGTH/2))
   {
       PTU |= BLUE_LED;
       PTU &= ~RED_LED;
       Red = False;
   }else
   {
      //printf("Side cannot be determined.");
   }
      
}