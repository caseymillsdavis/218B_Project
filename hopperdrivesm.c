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
#include <math.h>
#include <stdlib.h>
#include "S12eVec.h"
 

/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "HopperDriveSM.h"
//#include "HopperCalibrationDriveSM.h"
#include "TankSM.h"

#include "MotorDrivers.h"
#include "GameConstants.h"

/*----------------------------- Module Defines ----------------------------*/
// define constants for the states for this machine
// and any other local defines

#define BASE_DUTY 85
#define ZERO_SPEED 0
#define ROTATION_TIMER 5 // THIS NEEDS TO BE MOVED UP TO TANK SM, PROBABLY
//#define DUMP_TIMER 6 // THIS NEEDS TO BE MOVED UP TO TANK SM, PROBABLY
#define BACKWARD_TICKS_TO_DUMP_POSITION 110

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine, things like during
   functions, entry & exit functions.They should be functions relevant to the
   behavior of this state machine
*/
static ES_Event DuringHopperActive( ES_Event Event);
static ES_Event DuringGoingToHighPosition( ES_Event Event);
static ES_Event DuringHopperInactive( ES_Event Event);
static ES_Event DuringDumping( ES_Event Event);
void HopperInit(void);

void interrupt _Vec_tim1ch5 HopperEncoderRight(void);
void interrupt _Vec_tim1ch6 HopperEncoderLeft(void);

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well
static HopperState_t CurrentState;
static int HopperCountRight=0;
static int HopperCountLeft=0;
static int DeltaSpeed;
static int DeltaHistory;
static char LeftDuty;
static char RightDuty;

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
ES_Event RunHopperDriveSM( ES_Event CurrentEvent )
{
   unsigned char MakeTransition = False;/* are we making a state transition? */
   HopperState_t NextState = CurrentState;
   ES_Event EntryEventKind = { ES_ENTRY, 0 };// default to normal entry to new state
   ES_Event ReturnEvent = CurrentEvent; // assume we are not consuming event
   
if(QueryTankSM() == CALIBRATING_HOPPER)
{

switch ( CurrentState )
   {
       /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/
       case HOPPER_ON :       // If current state is state one
       /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/
       
         //LEFT_OR_REAR_HOPPER_PWM=BASE_DUTY;
         //RIGHT_OR_FRONT_HOPPER_PWM=BASE_DUTY;
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
       /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/
         CurrentEvent = DuringHopperActive(CurrentEvent);
       /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
         //printf("Got an event!\n\r");
            switch (CurrentEvent.EventType)
            {

				case BUTTON_PRESSED :   //LEFT_ZEROED: //If event is event one
       /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/
            {
            
                  LeftDuty = 0;
                  LEFT_OR_REAR_HOPPER_PWM=LeftDuty;
                  //printf("stopping left.\n\r");
                  // Execute action function for state one : event one
                  NextState = LEFT_OFF_WAITING_FOR_RIGHT; //Decide what the next state will be
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
       /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/  

      
		 case LEFT_OFF_WAITING_FOR_RIGHT :       // If current state is state one
       /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         CurrentEvent = DuringGoingToHighPosition(CurrentEvent);
         //process any events
       /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
       /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/
         {
            switch (CurrentEvent.EventType)
            {           
       /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/
               case BUTTON_PRESSED :    //RIGHT_ZEROED : //If event is event one
                  {
                  //ES_Event EndEvent;
                  //EndEvent.EventType = CALIBRATION_COMPLETE;
                  //EndEvent.EventParam = 1;
                  //ES_PostList00(EndEvent);   
 
       /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/
                  RightDuty = 0;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty;
                  
                  
                  // THIS IS THE NEW LOW POSITION, OR "ZERO".
                  HopperCountRight = 0;
                  HopperCountLeft = 0;
                  
                  //printf("stopping right.\n\r");
                  //Keep Rotating Until We Get to High Position
                  
                  
                  // Execute action function for state one : event one
                  NextState = WAIT_FOR_BOX_LOAD; //Decide what the next state will be
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
        /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/
       
        /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/
		 case HOPPER_OFF :       // If current state is state one
       /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         CurrentEvent = DuringGoingToHighPosition(CurrentEvent);
         //process any events
       /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
       /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/
         {
            switch (CurrentEvent.EventType)
            {           
       /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/
               case BUTTON_PRESSED :  // START_CALIBRATING_HOPPER : //If event is event one
       /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/
                  RightDuty = 35;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty;
                  LeftDuty = 30;
                  LEFT_OR_REAR_HOPPER_PWM=LeftDuty;
                  //Keep Rotating Until We Get to High Position
                  // Execute action function for state one : event one
                  NextState = HOPPER_ON; //Decide what the next state will be
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
         break;
         
       case WAIT_FOR_BOX_LOAD :       // If current state is state one
       /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         CurrentEvent = DuringGoingToHighPosition(CurrentEvent);
         //process any events
       /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
       /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/
         {
            switch (CurrentEvent.EventType)
            {           
       /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/
               case BUTTON_PRESSED :  // START_CALIBRATING_HOPPER : //If event is event one
       /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/
                  RightDuty = BASE_DUTY;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty;
                  LeftDuty = BASE_DUTY;
                  LEFT_OR_REAR_HOPPER_PWM=LeftDuty;
                  //Keep Rotating Until We Get to High Position
                  // Execute action function for state one : event one
                  NextState = DYNAMIC_CALIBRATION; //Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;   
               case ZERO_FOUND : //WALL_THREATENS_HOPPER : //If event is event one
                  {                                       
                  //ES_Event EndEvent;
                  //EndEvent.EventType = CALIBRATION_COMPLETE;
                  //EndEvent.EventParam = 1;
                  //ES_PostList00(EndEvent);   

                  LeftDuty = 0;
                  LEFT_OR_REAR_HOPPER_PWM=LeftDuty;
                  RightDuty = 0;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty;
                  //Keep Rotating Until We Get to High Position
                  // Execute action function for state one : event one
                  NextState = WAIT_FOR_END_CALIBRATION; //Decide what the next state will be
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
         
         case DYNAMIC_CALIBRATION :       // If current state is state one
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         CurrentEvent = DuringHopperActive(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
               case BUTTON_PRESSED : //WALL_THREATENS_HOPPER : //If event is event one
                  {                                       
                  //ES_Event EndEvent;
                  //EndEvent.EventType = CALIBRATION_COMPLETE;
                  //EndEvent.EventParam = 1;
                  //ES_PostList00(EndEvent);   

                  //LEFT_OR_REAR_HOPPER_PWM=BASE_DUTY;
                  //RIGHT_OR_FRONT_HOPPER_PWM=BASE_DUTY;
                  //Keep Rotating Until We Get to High Position
                  // Execute action function for state one : event one
                  NextState = HIGH_CALIBRATE_NEITHER; //Decide what the next state will be
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
				case VCONTROL_UPDATE: //If event is event one
            {
            
                  VelocityControl();
                  // Execute action function for state one : event one
                  NextState = DYNAMIC_CALIBRATION; //Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
            }
                  break;
  
                       
            case SCONTROL_UPDATE: //If event is event one
            {
                  StallControl(CurrentEvent.EventParam);
                  // Execute action function for state one : event one
                  NextState = DYNAMIC_CALIBRATION; //Decide what the next state will be
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

            }
         }
         break;
         
         
         case WAIT_FOR_END_CALIBRATION :       // If current state is state one
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         CurrentEvent = DuringHopperActive(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
               case BUTTON_PRESSED : //WALL_THREATENS_HOPPER : //If event is event one
                  {                                       
                  ES_Event EndEvent;
                  EndEvent.EventType = CALIBRATION_COMPLETE;
                  EndEvent.EventParam = 1;
                  ES_PostList00(EndEvent);   

                  LeftDuty = 0;
                  LEFT_OR_REAR_HOPPER_PWM=LeftDuty;
                  RightDuty = 0;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty;
                  //Keep Rotating Until We Get to High Position
                  // Execute action function for state one : event one
                  NextState = HOPPER_INACTIVE; //Decide what the next state will be
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
         case HIGH_CALIBRATE_NEITHER :       // If current state is state one
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         CurrentEvent = DuringGoingToHighPosition(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            
              //RIGHT_OR_FRONT_HOPPER_PWM=BASE_DUTY;
              //LEFT_OR_REAR_HOPPER_PWM =BASE_DUTY;
            switch (CurrentEvent.EventType)
            {
               case HIGH_POSITION_REACHED_RIGHT : //If event is event one
                  
                  //StopHopper(); //Should be redundant
                  RightDuty = 0;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty;
                  LeftDuty = 0;//////////////////////////////////////////////////
                  LEFT_OR_REAR_HOPPER_PWM=LeftDuty;
                  //IM ASSUMING CHANNEL 2 GOES TO RIGHT DRIVER
                  // Execute action function for state one : event one
                  NextState = WAIT_FOR_BOX_LOAD;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
                // repeat cases as required for relevant events               
               case HIGH_POSITION_REACHED_LEFT : //If event is event one
                  
                  //StopHopper(); //Should be redundant
                  LeftDuty = 0;
                  LEFT_OR_REAR_HOPPER_PWM=LeftDuty;//IM ASSUMING CHANNEL 2 GOES TO RIGHT DRIVER
                  RightDuty = 100;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty;
                  // Execute action function for state one : event one
                  NextState = HIGH_CALIBRATE_LEFT_STOPPED;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
               case VCONTROL_UPDATE : //If event is event one
                  
				      VelocityControl();
                  // Execute action function for state one : event one
                  
                  break;
 
               
               case SCONTROL_UPDATE : //If event is event one
                  
				      StallControl(CurrentEvent.EventParam);
                  
                  break;   
            }
         }
         break;
		 
		 case HIGH_CALIBRATE_RIGHT_STOPPED :
      // If current state is state one
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         CurrentEvent = DuringGoingToHighPosition(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            
              //LEFT_OR_REAR_HOPPER_PWM =100;
           
            switch (CurrentEvent.EventType)
            {           
               case HIGH_POSITION_REACHED_LEFT : //If event is event one
                  
                  //StopHopper(); //Should be redundant
                  LeftDuty = 0;
                  LEFT_OR_REAR_HOPPER_PWM=LeftDuty;//IM ASSUMING CHANNEL 3 GOES TO LEFT DRIVER
                  // Execute action function for state one : event one
                  NextState = WAIT_FOR_BOX_LOAD;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
  
				case SCONTROL_UPDATE : //If event is event one
				      if (CurrentEvent.EventParam == LEFT)
				      {
				         StallControl(CurrentEvent.EventParam);   
				      }

                  break;                     
            }
         }
         break;
         
		 case HIGH_CALIBRATE_LEFT_STOPPED :       // If current state is state one
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         CurrentEvent = DuringGoingToHighPosition(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            //RIGHT_OR_FRONT_HOPPER_PWM=100;

            switch (CurrentEvent.EventType)
            {           
               case HIGH_POSITION_REACHED_RIGHT : //If event is event one
                  
                  //StopHopper(); //Should be redundant
                  RightDuty = 0;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty;//IM ASSUMING CHANNEL 2 GOES TO RIGHT DRIVER
                  // Execute action function for state one : event one
                  NextState = WAIT_FOR_BOX_LOAD;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;

             case SCONTROL_UPDATE : //If event is event one
				      if (CurrentEvent.EventParam == RIGHT)
				      {
				         StallControl((char)CurrentEvent.EventParam);   
				      }

                  break;   
            }
         }
         break;
        /****************************************************************************************
       CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!   CALIBRATION ROUTINE!
       ****************************************************************************************/
   }


} else
{
   
    //printf("Event3: %d /n/r", CurrentEvent.EventType);
   switch ( CurrentState )
   {
       case HOPPER_ACTIVE :       // If current state is state one
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         CurrentEvent = DuringHopperActive(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
               case WALL_THREATENS_HOPPER : //If event is event one
                  //printf("we got the stop command.\n\r");
                  //Keep Rotating Until We Get to High Position
                  // Execute action function for state one : event one
                  NextState = GOING_TO_HIGH_POSITION_NEITHERHIGH; //Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
                // repeat cases as required for relevant events
				case VCONTROL_UPDATE: //If event is event one
            {
            
                  VelocityControl();
                  // Execute action function for state one : event one
                  NextState = HOPPER_ACTIVE; //Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
            }
                  break;
                  
            case SCONTROL_UPDATE: //If event is event one
            {
                  StallControl((char)CurrentEvent.EventParam);
                  // Execute action function for state one : event one
                  NextState = HOPPER_ACTIVE; //Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
            }
                  break;
            /*
            case GO_TO_LOW_COMMAND :
                  
                  //DumpRoutine();
                  LeftDuty = BASE_DUTY;
                  LEFT_OR_REAR_HOPPER_PWM=LeftDuty;
                  RightDuty = BASE_DUTY;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty;
                  // Execute action function for state one : event one
                  NextState = GOING_TO_LOW_POSITION_NEITHERLOW;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;   
             
             case START_CALIBRATING_HOPPER :
                  
                  //DumpRoutine();
                  RightDuty = 50;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty;
                  LeftDuty = 50;
                  LEFT_OR_REAR_HOPPER_PWM=LeftDuty;
                  // Execute action function for state one : event one
                  NextState = GOING_TO_LOW_POSITION_NEITHERLOW;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;      
             */          
            
                // repeat cases as required for relevant events

            }
         }
         break;
         

         
         case GOING_TO_HIGH_POSITION_NEITHERHIGH :       // If current state is state one
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         CurrentEvent = DuringGoingToHighPosition(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
               case HIGH_POSITION_REACHED_RIGHT : //If event is event one
                  
                  //StopHopper(); //Should be redundant
                  RightDuty = 0;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty;
                  LeftDuty = 0;
                  LEFT_OR_REAR_HOPPER_PWM=LeftDuty;
                  //IM ASSUMING CHANNEL 2 GOES TO RIGHT DRIVER
                  // Execute action function for state one : event one
                  NextState = HOPPER_INACTIVE;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
                // repeat cases as required for relevant events               
               case HIGH_POSITION_REACHED_LEFT : //If event is event one
                  
                  //StopHopper(); //Should be redundant
                  LeftDuty = 0;
                  LEFT_OR_REAR_HOPPER_PWM=LeftDuty;//IM ASSUMING CHANNEL 2 GOES TO RIGHT DRIVER
                  RightDuty = 100;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty;
                  // Execute action function for state one : event one
                  NextState = GOING_TO_HIGH_POSITION_LEFTHIGH;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
               case VCONTROL_UPDATE : //If event is event one
                  
				      VelocityControl();
                  // Execute action function for state one : event one
                  NextState = GOING_TO_HIGH_POSITION_NEITHERHIGH;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
               /*
               case GO_TO_LOW_COMMAND :
                  
                  //DumpRoutine();
                  LeftDuty = BASE_DUTY;
                  LEFT_OR_REAR_HOPPER_PWM=LeftDuty;
                  RightDuty = BASE_DUTY;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty;
                  // Execute action function for state one : event one
                  NextState = GOING_TO_LOW_POSITION_NEITHERLOW;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;  
               */
               case SCONTROL_UPDATE : //If event is event one
                  
				      StallControl((char)CurrentEvent.EventParam);
                  // Execute action function for state one : event one
                  NextState = GOING_TO_HIGH_POSITION_NEITHERHIGH;//Decide what the next state will be
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
         break;
		 
		 case GOING_TO_HIGH_POSITION_RIGHTHIGH :       // If current state is state one
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         CurrentEvent = DuringGoingToHighPosition(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {           
               case HIGH_POSITION_REACHED_LEFT : //If event is event one
                  
                  //StopHopper(); //Should be redundant
                  LeftDuty = 0;
                  LEFT_OR_REAR_HOPPER_PWM=LeftDuty;//IM ASSUMING CHANNEL 3 GOES TO LEFT DRIVER
                  // Execute action function for state one : event one
                  NextState = HOPPER_INACTIVE;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
				case GO_TO_LOW_COMMAND :
                  
                  //DumpRoutine();
                  LeftDuty = BASE_DUTY;
                  LEFT_OR_REAR_HOPPER_PWM=LeftDuty;
                  RightDuty = BASE_DUTY;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty;
                  // Execute action function for state one : event one
                  NextState = GOING_TO_LOW_POSITION_NEITHERLOW;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;  
				case SCONTROL_UPDATE : //If event is event one
				      if (CurrentEvent.EventParam == LEFT)
				      {
				         StallControl((char)CurrentEvent.EventParam);   
				      }
                  // Execute action function for state one : event one
                  NextState = GOING_TO_HIGH_POSITION_RIGHTHIGH;//Decide what the next state will be
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
         break;
         
		 case GOING_TO_HIGH_POSITION_LEFTHIGH :       // If current state is state one
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         CurrentEvent = DuringGoingToHighPosition(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {           
               case HIGH_POSITION_REACHED_RIGHT : //If event is event one
                  
                  //StopHopper(); //Should be redundant
                  RightDuty = 0;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty;//IM ASSUMING CHANNEL 2 GOES TO RIGHT DRIVER
                  // Execute action function for state one : event one
                  NextState = HOPPER_INACTIVE;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
				case GO_TO_LOW_COMMAND :
                  
                  //DumpRoutine();
                  LeftDuty = BASE_DUTY;
                  LEFT_OR_REAR_HOPPER_PWM=LeftDuty;
                  RightDuty = BASE_DUTY;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty;
                  // Execute action function for state one : event one
                  NextState = GOING_TO_LOW_POSITION_NEITHERLOW;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;  
             case SCONTROL_UPDATE : //If event is event one
				      if (CurrentEvent.EventParam == RIGHT)
				      {
				         StallControl((char)CurrentEvent.EventParam);   
				      }
                  // Execute action function for state one : event one
                  NextState = GOING_TO_HIGH_POSITION_LEFTHIGH;//Decide what the next state will be
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
         break;
		   case GOING_TO_LOW_POSITION_NEITHERLOW :       // If current state is state one
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         CurrentEvent = DuringGoingToHighPosition(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
               case LOW_POSITION_REACHED_RIGHT : //If event is event one
                  
                  //StopHopper(); //Should be redundant
                  RightDuty = 0;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty;//IM ASSUMING CHANNEL 2 GOES TO RIGHT DRIVER
                  // Execute action function for state one : event one
                  NextState = GOING_TO_LOW_POSITION_RIGHTLOW;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
                // repeat cases as required for relevant events               
               case LOW_POSITION_REACHED_LEFT : //If event is event one
                  
                  //StopHopper(); //Should be redundant
                  LeftDuty = 0;
                  LEFT_OR_REAR_HOPPER_PWM=LeftDuty;//IM ASSUMING CHANNEL 2 GOES TO RIGHT DRIVER
                  // Execute action function for state one : event one
                  NextState = GOING_TO_LOW_POSITION_LEFTLOW;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
               case VCONTROL_UPDATE : //If event is event one
                  
				      VelocityControl();
                  // Execute action function for state one : event one
                  NextState = GOING_TO_LOW_POSITION_NEITHERLOW;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
               case SCONTROL_UPDATE : //If event is event one
                  
				      StallControl((char)CurrentEvent.EventParam);
                  // Execute action function for state one : event one
                  NextState = GOING_TO_LOW_POSITION_NEITHERLOW;//Decide what the next state will be
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
         break;
		 
		 case GOING_TO_LOW_POSITION_RIGHTLOW :       // If current state is state one
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         CurrentEvent = DuringGoingToHighPosition(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {           
               case LOW_POSITION_REACHED_LEFT : //If event is event one
                  
                  //StopHopper(); //Should be redundant
                  LeftDuty = 0;
                  LEFT_OR_REAR_HOPPER_PWM=LeftDuty;//IM ASSUMING CHANNEL 3 GOES TO LEFT DRIVER
                  // Execute action function for state one : event one
                  NextState = HOPPER_INACTIVE;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
				/*
				case VCONTROL_UPDATE : //If event is event one
                  
				      VelocityControl();
                  // Execute action function for state one : event one
                  NextState = GOING_TO_HIGH_POSITION_RIGHTHIGH;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
				*/
				case SCONTROL_UPDATE : //If event is event one
				      if (CurrentEvent.EventParam == LEFT)
				      {
				         StallControl((char)CurrentEvent.EventParam);   
				      }
                  // Execute action function for state one : event one
                  NextState = GOING_TO_LOW_POSITION_RIGHTLOW;//Decide what the next state will be
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
         break;
         
		 case GOING_TO_LOW_POSITION_LEFTLOW :       // If current state is state one
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         CurrentEvent = DuringGoingToHighPosition(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {           
               case LOW_POSITION_REACHED_RIGHT : //If event is event one
                  
                  //StopHopper(); //Should be redundant
                  RightDuty = 0;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty;//IM ASSUMING CHANNEL 2 GOES TO RIGHT DRIVER
                  // Execute action function for state one : event one
                  NextState = HOPPER_INACTIVE;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
				/*
				case VCONTROL_UPDATE : //If event is event one
                  
				      VelocityControl();
                  // Execute action function for state one : event one
                  NextState = GOING_TO_HIGH_POSITION_LEFTHIGH;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
                  
             */
             case SCONTROL_UPDATE : //If event is event one
				      if (CurrentEvent.EventParam == RIGHT)
				      {
				         StallControl((char)CurrentEvent.EventParam);   
				      }
                  // Execute action function for state one : event one
                  NextState = GOING_TO_LOW_POSITION_LEFTLOW;//Decide what the next state will be
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
         break;
         case HOPPER_INACTIVE :       // If current state is state one
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         CurrentEvent = DuringHopperInactive(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
               case WALL_NO_LONGER_THREATENS_HOPPER : //If event is event one
                  
                  RightDuty = 100;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty;//BASE_DUTY;
				      LeftDuty = 100;
                  LEFT_OR_REAR_HOPPER_PWM=LeftDuty;//BASE_DUTY;
                  
                  //DriveHopper();
                  
                  // Execute action function for state one : event one
                  NextState = HOPPER_ACTIVE;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
                // repeat cases as required for relevant events 


               case DEPOSIT_COMMAND : //If event is event one
                 // printf("DUMPING\n\r");
                  //DumpRoutine();
                  HopperCountRight = 0;
                  
                  RightDuty = 98;//BASE_DUTY;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty;
                  // Execute action function for state one : event one
                  NextState = DUMPING;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
                // repeat cases as required for relevant events
                
               case GO_TO_LOW_COMMAND :
                  
                  //DumpRoutine();
                  LeftDuty = BASE_DUTY;
                  LEFT_OR_REAR_HOPPER_PWM=LeftDuty;
                  RightDuty = BASE_DUTY;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty;
                  // Execute action function for state one : event one
                  NextState = GOING_TO_LOW_POSITION_NEITHERLOW;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
                // repeat cases as required for relevant events

               case DUMP_POSITION_REACHED : //If event is event one
                  LeftDuty = BASE_DUTY;
                  LEFT_OR_REAR_HOPPER_PWM=LeftDuty;
                  RightDuty = BASE_DUTY;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty; 
                  
                  //DumpRoutine();
                  // Execute action function for state one : event one
                  NextState = HOPPER_ACTIVE;//Decide what the next state will be
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
         
		 case DUMPING :       // If current state is state one
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lowere level state machines to re-map
         // or consume the event
         CurrentEvent = DuringDumping(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {

                case DUMP_POSITION_REACHED : //If event is event one
                  RightDuty = 0;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty;
                  // Execute action function for state one : event one
                  NextState = DUMPING;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
                // repeat cases as required for relevant events 
                  
                case WALL_NO_LONGER_THREATENS_HOPPER :
                  
                  //DumpRoutine();
                  LeftDuty = BASE_DUTY;
                  LEFT_OR_REAR_HOPPER_PWM=LeftDuty;
                  RightDuty = BASE_DUTY;
                  RIGHT_OR_FRONT_HOPPER_PWM=RightDuty;
                  // Execute action function for state one : event one
                  NextState = HOPPER_ACTIVE;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = True; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent = CurrentEvent;
                  break;
                  
                case DEPOSIT_COMMAND :
                  //HopperCountLeft = 0;
                  //DumpRoutine();
                  LeftDuty = BASE_DUTY;
                  LEFT_OR_REAR_HOPPER_PWM=LeftDuty;
                  puts("You're going crazy\n\r");
                  // Execute action function for state one : event one
                  NextState = HOPPER_INACTIVE;//Decide what the next state will be
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
         break;
    }
    

}

    
    //   If we are making a state transition
    if (MakeTransition == True)
    {
       //   Execute exit function for current state
       CurrentEvent.EventType = ES_EXIT;
       RunHopperDriveSM(CurrentEvent);
        
       CurrentState = NextState; //Modify state variable
       
       //   Execute entry function for new state
       // this defaults to ES_ENTRY
       RunHopperDriveSM(EntryEventKind);
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
void StartHopperDriveSM ( ES_Event CurrentEvent )
{
   // local variable to get debugger to display the value of CurrentEvent
   ES_Event LocalEvent = CurrentEvent;
	
   if(QueryTankSM() == CALIBRATING_HOPPER)
        {
        HopperInit(); 
        }	 
	 
   // to implement entry to a history state or directly to a substate
   // you can modify the initialization of the CurrentState variable
   // otherwise just start in the entry state every time the state machine
   // is started
   if ( ES_ENTRY_HISTORY != CurrentEvent.EventType )
   {
        if(QueryTankSM() == CALIBRATING_HOPPER)
        {
        
        CurrentState = HOPPER_OFF; 
        } else
        {
        CurrentState = HOPPER_INACTIVE; 
        }
        //RightDuty = 0;
        //LeftDuty = 0;
   }
   // call the entry function (if any) for the ENTRY_STATE
   RunHopperDriveSM(CurrentEvent);
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
HopperState_t QueryHopperDriveSM ( void )
{
   return(CurrentState);
}

/***************************************************************************
 private functions
 ***************************************************************************/

static ES_Event DuringHopperActive( ES_Event Event)
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

static ES_Event DuringGoingToHighPosition( ES_Event Event)
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

static ES_Event DuringHopperInactive( ES_Event Event)
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

static ES_Event DuringDumping( ES_Event Event)
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


void HopperInit(void){

	TIM1_TSCR1 = _S12_TEN;							// enable timer 1
	TIM1_TSCR2 = _S12_PR0 | _S12_PR1 | _S12_PR2;	// prescale to 128
	TIM1_TIOS &= ~(_S12_IOS5|_S12_IOS6);			// set timer 1 pins 5 and 6 to input compares
	TIM1_TCTL3|=(_S12_EDG5A|_S12_EDG6A);				// respond to rising edges only
	TIM1_TIE |= _S12_C5I;							// enable timer 1 IC5 interrupt
	TIM1_TIE |= _S12_C6I;							// enable timer 1 IC6 interrupt
	TIM1_TFLG1 = _S12_C5F;							// clear timer 1 IC 5 flag
	TIM1_TFLG1 = _S12_C6F;							// clear timer 1 IC 6 flag
	
	EnableInterrupts;								// enable interrupts globally	
}


void interrupt _Vec_tim1ch5 HopperEncoderRight(void)
{
   ES_Event ThisEvent;
   int CurrentHopperCountRight;
   HopperCountRight++;
   printf("R: %d.",HopperCountRight);
   CurrentHopperCountRight=HopperCountRight+HOPPER_OFFSET+COUNTS_TO_HIGH;
   if((CurrentHopperCountRight+90)%180==0)
      {
         ThisEvent.EventType=HIGH_POSITION_REACHED_RIGHT;
         ES_PostList00(ThisEvent);
      }
      
   if((HopperCountRight-30)%180==0)
   {
      ThisEvent.EventType=DUMP_POSITION_REACHED;
   	ES_PostList00(ThisEvent);;
   }
   
   if((CurrentHopperCountRight)%180==0)
   {
      ThisEvent.EventType=LOW_POSITION_REACHED_RIGHT;
   	ES_PostList00(ThisEvent);
   }
   
   TIM1_TFLG1 = _S12_C5F;							// clear timer 1 IC 5 flag
}
void interrupt _Vec_tim1ch6 HopperEncoderLeft(void)
{
   ES_Event ThisEvent;
   int CurrentHopperCountLeft;
   HopperCountLeft++;
   printf("L: %d\n\r.",HopperCountLeft);
   CurrentHopperCountLeft=HopperCountLeft+COUNTS_TO_HIGH;
   //printf("Left Count is %d\n\r", HopperCountLeft);
   if((CurrentHopperCountLeft+90)%180==0)
   {
      ThisEvent.EventType=HIGH_POSITION_REACHED_LEFT;
      ES_PostList00(ThisEvent);;
   }
   
   if((CurrentHopperCountLeft+90-30)%180==0)
   {
      ThisEvent.EventType=DUMP_POSITION_REACHED;
   	ES_PostList00(ThisEvent);
   }
   
   if((CurrentHopperCountLeft)%180==0)
   {
      ThisEvent.EventType=LOW_POSITION_REACHED_LEFT;
   	ES_PostList00(ThisEvent);
   }

   TIM1_TFLG1 = _S12_C6F;							// clear timer 1 IC 6 flag
}



int GetRightCount(void)
{
   return(HopperCountRight);
}
int GetLeftCount(void)
{
   return(HopperCountLeft);
}
int GetDelta(void)
{
   return(DeltaSpeed);
}
int GetDeltaHistory(void)
{
   return(DeltaHistory);
}
unsigned char GetPWMLeftHopper(void)
{
   return (LeftDuty);   
}
unsigned char GetPWMRightHopper(void)
{
   return (RightDuty);   
}

void VelocityControl(void)
{
      int CurrentHopperCountRight=HopperCountRight+HOPPER_OFFSET;
      DeltaSpeed = (CurrentHopperCountRight-HopperCountLeft);
      printf("         Delta is %d\n\r", DeltaSpeed);
		DeltaHistory=DeltaHistory+DeltaSpeed;
		if(DeltaHistory>1000)
		{
		   DeltaHistory=1000;
		}else if(DeltaHistory<-1000)
		{
		   DeltaHistory=-1000;
		}
		RightDuty =((char)BASE_DUTY - PROPORTIONAL_CONSTANT*DeltaSpeed); //- DeltaHistory/INTEGRAL_CONSTANT);
		if(RightDuty>100)
		{
		   RightDuty=100;
		}else if(RightDuty<0)
		{
		   RightDuty=0;
		}
      LeftDuty = ((char)BASE_DUTY + PROPORTIONAL_CONSTANT*DeltaSpeed); //+ DeltaHistory/INTEGRAL_CONSTANT);
      if(LeftDuty>100)
      {
         LeftDuty=100;
      }else if(LeftDuty<0)
      {
         LeftDuty=0;
      }
		RIGHT_OR_FRONT_HOPPER_PWM= (unsigned char)RightDuty;
		LEFT_OR_REAR_HOPPER_PWM= (unsigned char)LeftDuty;
}

void StallControl(int KindOfStall)
{
  if(KindOfStall == RIGHT)
  {
   RightDuty=100;
   RIGHT_OR_FRONT_HOPPER_PWM= (unsigned char)RightDuty;
   LeftDuty=0;
   LEFT_OR_REAR_HOPPER_PWM= (unsigned char)LeftDuty;
   
  }else if (KindOfStall == LEFT)
  {
   LeftDuty=100;
   LEFT_OR_REAR_HOPPER_PWM= (unsigned char)LeftDuty ;
   RightDuty=0;
   RIGHT_OR_FRONT_HOPPER_PWM= (unsigned char)RightDuty;
   
  } else // this would be the "both" case
  {
   RightDuty=100;
   RIGHT_OR_FRONT_HOPPER_PWM= (unsigned char)RightDuty;
   LeftDuty=100;
   LEFT_OR_REAR_HOPPER_PWM= (unsigned char)LeftDuty; 
  }
}