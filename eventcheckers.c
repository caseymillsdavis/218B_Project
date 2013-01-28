// Event Checking functions for sample
#include "ES_Configure.h"
#include "ES_General.h"
#include "ES_Events.h"
#include "ES_PostList.h"
#include "EventCheckers.h"
#include "GameControlSM.h"
 
//#include "PositionControlSM.h"
#include "TreadDriveSM.h"
#include "HopperDriveSM.h"
#include "GameConstants.h"
#include "S12eVec.h"
#include "ES_Timers.h"
#include "IRSM.h"
#include "ADS12.h"


#include <hidef.h>         /* common defines and macros */ 
#include <mc9s12e128.h>    /* derivative information */ 
#include <S12e128bits.h>   /* bit definitions  */ 
#include <Bin_Const.h>     /* macros to allow specifying binary constants */ 
#include <termio.h>        /* to get prototype for kbhit() */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>


// This include will pull in all of the headers from the service modules
// providing the prototypes for all of the post functions
#include "ES_ServiceHeaders.h"
#include "QuerySM.h"   //redundant?
#include "PathPlannerModule.h"

#define THINKTANKEVENT 0;
#define TANKEVENT 1;

// Module functions (Interrupts)
void interrupt _Vec_tim0ch4 IRInterruptL(void);
void interrupt _Vec_tim0ch5 IRInterrupt(void);

// Module variables
// IR/Stepper variables
static boolean LastHoneStatus_Bin1;
static boolean LastRStatus_Bin1;
static boolean LastLStatus_Bin1;
static boolean LastHoneStatus_Bin2;
static boolean LastRStatus_Bin2;
static boolean LastLStatus_Bin2;
static boolean LastHoneStatus_Bin3;
static boolean LastRStatus_Bin3;
static boolean LastLStatus_Bin3;
static boolean LastHoneStatus_Bin4;
static boolean LastRStatus_Bin4;
static boolean LastLStatus_Bin4;
static boolean LastAnyBeacons;
static boolean LastTapeStatus;
// I changed these to static <change/>
static boolean Beacon_Bin1_R;
static boolean Beacon_Bin1_L;
static boolean Beacon_Bin2_R;
static boolean Beacon_Bin2_L;
static boolean Beacon_Bin3_R;
static boolean Beacon_Bin3_L;
static boolean Beacon_Bin4_R;
static boolean Beacon_Bin4_L;
// </end change>
static int BeaconArray[4] = {3750, 3375, 3000, 2625};
static unsigned int uLastPulse = 0;
static unsigned int uPeriod = 0;
static unsigned int uLastPulseL = 0;
static unsigned int uPeriodL = 0;
// end IR/stepper variables

// for post lists for key stroke checker
static boolean IsTankEvent;
static boolean IsIR_StepperEvent;
static boolean IsThinkTankEvent;

static unsigned int LastBallStatus;

static unsigned int LastBin1Status;
static unsigned int LastBin2Status;
static unsigned int LastBin3Status;
static unsigned int LastBin4Status;

static unsigned int LastWallAngleStatus;


static int LastLeft;
static int LastRight;
static int LastTimeRightChange;
static int LastTimeLeftChange;
static int LastDelta;
static boolean RightIsStalled;
static boolean LeftIsStalled;


static float LastXPosition;
static float LastYPosition;
static float LastBotOrientation;

static boolean WallThreatState;

static short lastCountsToHighValue=0;
static short lastOffsetValue=0;

static unsigned int TimeOfLastButtonSample;
static char LastButtonState;
  

void InitializeCheckers( void)
{
	LastBallStatus = 0x00;

	LastBin1Status = 0x00;
	LastBin2Status = 0x00;
	LastBin3Status = 0x00;
	LastBin4Status = 0x00;

	LastWallAngleStatus = 0x00;
	LastTimeRightChange = ES_Timer_GetTime();
   LastTimeLeftChange = ES_Timer_GetTime();
	
	// Stepper/IR init
	LastHoneStatus_Bin1 = False;
	LastRStatus_Bin1 = False;
	LastLStatus_Bin1 = False;
	LastHoneStatus_Bin2 = False;
	LastRStatus_Bin2 = False;
	LastLStatus_Bin2 = False;
	LastHoneStatus_Bin3 = False;
	LastRStatus_Bin3 = False;
	LastLStatus_Bin3 = False;
	LastHoneStatus_Bin4 = False;
	LastRStatus_Bin4 = False;
	LastLStatus_Bin4 = False;
	LastAnyBeacons = False;
	LastTapeStatus = False;
	
   Beacon_Bin1_R = False;
   Beacon_Bin1_L = False;
   Beacon_Bin2_R = False;
   Beacon_Bin2_L = False;
   Beacon_Bin3_R = False;
   Beacon_Bin3_L = False;
   Beacon_Bin4_R = False;
   Beacon_Bin4_L = False;
   
   LastLeft = 0;
   LastRight = 0;
   
   LastXPosition = 0;
   LastYPosition = 0;
   LastBotOrientation = 0;
   
   LastLeft = GetLeftCount();
   LastRight = GetRightCount();
   LastTimeRightChange=ES_Timer_GetTime();
   LastTimeLeftChange=ES_Timer_GetTime();
   LastDelta=LastRight-LastLeft;
   RightIsStalled=False;
   LeftIsStalled=False;
   
   WallThreatState = False;
   
   
   LastButtonState = PTP&BIT0HI;
   TimeOfLastButtonSample = ES_Timer_GetTime();
   
	
}

boolean UpdateADPins(void)
{

   //puts("Trying to update AD pins");
   short currentCountsToHighValue=ADS12_ReadADPin(0);
   short currentOffsetValue=ADS12_ReadADPin(1);
   
   if((currentCountsToHighValue-lastCountsToHighValue>PROP_THRESHOLD)||(lastCountsToHighValue-currentCountsToHighValue>PROP_THRESHOLD))
   {
     COUNTS_TO_HIGH=(char)(currentCountsToHighValue/10-50);
     lastCountsToHighValue=currentCountsToHighValue;
     printf("CTH:%d\n\r",COUNTS_TO_HIGH);
   }
   if((currentOffsetValue-lastOffsetValue>OFFSET_THRESHOLD)||(lastOffsetValue-currentOffsetValue>OFFSET_THRESHOLD))
   {
     HOPPER_OFFSET=(signed char)(currentOffsetValue/10-50);
     lastOffsetValue=currentOffsetValue;
     printf("HOF:%d\n\r",HOPPER_OFFSET);
   }
   
   
   
   return False;
}


boolean CheckButtonEvents(void)
{
//Local ReturnVal = False
boolean ReturnVal = False;
unsigned int CurrentTime = ES_Timer_GetTime();
//Get current button state
char CurrentButtonState = PTP&BIT0HI;
//If more than the debounce interval has elapsed since the last sampling
if((CurrentTime-TimeOfLastButtonSample)>DEBOUNCE_TIME)
{
//puts("latch");
	//If the current button state is different from the LastButtonState
   if ((CurrentButtonState != LastButtonState) &&(CurrentButtonState != 0))
   {
         
			//PostEvent ButtonDown (whether down or up)
			ES_Event ThisEvent;
         ThisEvent.EventType = BUTTON_PRESSED;
         ThisEvent.EventParam = 1;
         ES_PostList00(ThisEvent);
         //printf("button fired.\n\r");
      	//Record current time as TimeOfLastSample
	      TimeOfLastButtonSample = ES_Timer_GetTime();   
         

	//Set ReturnVal = True
	ReturnVal = True;
	
	//Set LastButtonState to the current button state

	//Endif
   }
   
 

} 
// update variables
LastButtonState = CurrentButtonState;  
//Return ReturnVal
return ReturnVal;
//End of CheckButtonEvents
}


boolean KeyStrokeEvents(void)
{
    ES_Event ThisEvent;
    
    int KeyStroke;

    ThisEvent.EventType = ES_NO_EVENT;

    if ( kbhit() != 0)       // there was a key pressed
    {
        
        KeyStroke = getchar();
        switch ( toupper(KeyStroke))
        {
            case 'A' : ThisEvent.EventType = HONE_BIN1;
                       IsIR_StepperEvent= True; break;
            case 'B' : ThisEvent.EventType = HONE_BIN2; 
                       IsIR_StepperEvent= True; break;					   
            case 'C' : ThisEvent.EventType = HONE_BIN3; 
                       IsIR_StepperEvent= True; break;
            case 'D' : ThisEvent.EventType = HONE_BIN4;
                       IsIR_StepperEvent= True; break;              
			   case 'E' : ThisEvent.EventType = STOP_STEPPING; 
					        IsIR_StepperEvent= True; break; 
			   case 'F' : ThisEvent.EventType = DEPOSIT_TIME; 
                       IsThinkTankEvent = True; break;
            case 'G' : ThisEvent.EventType = FAVORITE_BIN_THREATENED;
                       IsThinkTankEvent = True; break;
            case 'H' : ThisEvent.EventType = BIN_CHOSEN; 
                       ThisEvent.EventParam = 1; 
                       IsThinkTankEvent = True; break;
            case 'I' : ThisEvent.EventType = DEPOSIT_COMMAND; 
                       /*IsTankEvent = True;*/ break;
            case 'J' : ThisEvent.EventType = WALL_CHOSEN;
                       ThisEvent.EventParam = LEFT;
                       IsThinkTankEvent = True; break;
            case 'K' : ThisEvent.EventType = EPIC_PUSH_FAIL; 
                       IsThinkTankEvent = True; break;
            case 'L' : ThisEvent.EventType = GOAL_ANGLE_ACHIEVED; 
                       IsThinkTankEvent = True; break;
            case 'M' : ThisEvent.EventType = WALL_THREATENS_HOPPER; 
                       IsTankEvent = True; break; 
            case 'N' : ThisEvent.EventType = WALL_NO_LONGER_THREATENS_HOPPER; 
                       IsTankEvent = True; break;
            case 'O' : ThisEvent.EventType = RIGHT_ZEROED;
                       IsTankEvent = True; break;
            case 'P' : ThisEvent.EventType = LEFT_ZEROED; 
                       IsTankEvent = True; break;
            case 'Q' : ThisEvent.EventType = START_CALIBRATING_HOPPER; 
                       IsTankEvent = True; break;
            case 'R' : ThisEvent.EventType = ARRIVED; 
                       IsThinkTankEvent = True; break;
            case 'S' : ThisEvent.EventType = ROTATION_COMPLETE; 
                       IsThinkTankEvent = True; break; 
            case 'T' : ThisEvent.EventType = GO_TO_LOW_COMMAND;
                       break;
            case 'U' : ThisEvent.EventType = BALLS_IN_PLAY;
                       break;                       
            case 'X' : ThisEvent.EventType = POSITION_FOUND; 
                       /*IsThinkTankEvent = True;*/ break;
			   case 'Y' : ThisEvent.EventType = FIND_POSITION; 
                       IsIR_StepperEvent= True; break;
			   case 'Z' : ThisEvent.EventType = ZERO_FOUND; 
                       break;		   
        }
        
        if (IsTankEvent)
        {
        ES_PostList00(ThisEvent); 
		  IsTankEvent = False;
        } else if (IsIR_StepperEvent)
		  {
	     ES_PostList06(ThisEvent);
		  IsIR_StepperEvent = False;		
		  }
		  else if (IsThinkTankEvent)
        {
        ES_PostList01(ThisEvent);
        IsThinkTankEvent = False;
		  }
		  else
		  {
		  ES_PostList02(ThisEvent);
		  ES_PostList06(ThisEvent);
		  }
        printf("posted\n\r");
        return(True);
    }
   
    return(False);
}

// sees if the wall threatens our hopper
boolean Check4WallProximity(void)
{
   /*
   boolean WallCurrentlyThreatensHopper;
   // could do an update control with a more advanced pass function if I wanted more "real-time" updates
   // definitely do that post check-off
   float _X_ = GetX();
   float _Y_ = GetY();
   float _Q_ = GetTheta();
   int WallQ = GetWallAngle();
   ES_Event ThreatEvent;
   
   if (CurrentLocationIsSafe(_X_, _Y_, _Q_, WallQ)== True)
   {
      WallCurrentlyThreatensHopper = False;
   }  else
   {
      WallCurrentlyThreatensHopper = True;
   }
   
   if (WallCurrentlyThreatensHopper != WallThreatState)
   {
      //what kind of event?
      if (WallCurrentlyThreatensHopper == True)
      {
         ThreatEvent.EventType = WALL_THREATENS_HOPPER;
         ThreatEvent.EventParam = 1;
         printf("Wall threatens us.\n\r");
            
      } else
      {
         ThreatEvent.EventType = WALL_NO_LONGER_THREATENS_HOPPER;
         ThreatEvent.EventParam = 1;
         printf("Wall no longer threatens us.\n\r");
      }
      //post event
      ES_PostList00(ThreatEvent);
      // update variable.
      WallThreatState = WallCurrentlyThreatensHopper;
      // return True
      return True;
   } else
   {
      return False;
   }
   */
   return False;
      
}



//#ifdef RUN_HOPPER_CALIBRATION_ROUTINE
/*
boolean Check4HopperDelta(void)
{
   return False;
}
*/
//#else
boolean Check4HopperStall(void)
{  

   // local current variables for comparisons.
   int CurrentLeft = GetLeftCount();
   int CurrentRight = GetRightCount();
   boolean RightIsCurrentlyStalled=False;
   boolean LeftIsCurrentlyStalled=False;
   int CurrentTime=ES_Timer_GetTime();
   
   // if the left count has changed, note the time.
   if (CurrentLeft!=LastLeft)
   {
      LastTimeLeftChange = ES_Timer_GetTime();
      LastLeft = CurrentLeft;
   }
   // if the right count has changed, note the time.
   if (CurrentRight!=LastRight)
   {
      LastTimeRightChange = ES_Timer_GetTime();
      LastRight = CurrentRight;
   }
   
   // if the time since the last change in a moter is greater than
   // stall time, we'll say that the motor is in a stall state, 
   // provided that we are actively trying to drive it with a non-zero
   // PWM value.
   
   if(((CurrentTime-LastTimeRightChange)>STALL_TIME)&&(GetPWMRightHopper()>10))
   {
      RightIsCurrentlyStalled = True;    
   } else
   {
      RightIsCurrentlyStalled = False;
   }
   
   if (((CurrentTime-LastTimeLeftChange)>STALL_TIME)&&(GetPWMRightHopper()>10))
   {
      LeftIsCurrentlyStalled = True; 
      //printf(".");  
   } else
   {
      LeftIsCurrentlyStalled = False;
   }

   // if either stall state has changed
   if (((RightIsCurrentlyStalled!=RightIsStalled) || (LeftIsCurrentlyStalled!=LeftIsStalled))    ) 
         //&& ((QueryHopperDriveSM()==GOING_TO_HIGH_POSITION_NEITHERHIGH)||
         //(QueryHopperDriveSM()==GOING_TO_HIGH_POSITION_RIGHTHIGH)||
         //(QueryHopperDriveSM()==GOING_TO_LOW_POSITION_NEITHERLOW)))
   {
      //printf(".");
      
      // Post the appropriate event.
      if ((RightIsCurrentlyStalled==True)&&(LeftIsCurrentlyStalled==True))
      {
         ES_Event StallEvent;
         StallEvent.EventType = SCONTROL_UPDATE;
         StallEvent.EventParam = BOTH;  
         ES_PostList00(StallEvent);   
      } else if (RightIsCurrentlyStalled==True)
      {
         ES_Event StallEvent;
         StallEvent.EventType = SCONTROL_UPDATE;
         StallEvent.EventParam = RIGHT;  
         ES_PostList00(StallEvent);   
      } else if (LeftIsCurrentlyStalled==True)
      {
         ES_Event StallEvent;
         StallEvent.EventType = SCONTROL_UPDATE;
         StallEvent.EventParam = LEFT;
         ES_PostList00(StallEvent);   
      } else
      {
         // Don't post a "no longer stalled" event.  I think just not posting has the same effect.
      }
      // Update Variables
      LeftIsStalled = LeftIsCurrentlyStalled;
      RightIsStalled = RightIsCurrentlyStalled;
      
      // Return true.
      return True;   
   } else
   {
      // no change to report.
      return False;
   }
   
}

boolean Check4HopperDelta(void)
{
   int CurrentDelta;
   int LeftCount = GetLeftCount();
   int RightCount = GetRightCount();
   
  
   
   CurrentDelta = RightCount + HOPPER_OFFSET - LeftCount ;
   
   // check for a change in delta, that delta is bigger than 1, and that the motors aren't
   // stalled (conflicting control law).
   if((CurrentDelta!=LastDelta)&&(abs(CurrentDelta)>1))//&&(RightIsStalled==False)&&(LeftIsStalled==False))
   {
    // Post Event.
    ES_Event ThisEvent;
    ThisEvent.EventType = VCONTROL_UPDATE;
    ThisEvent.EventParam = 1;
    ES_PostList00(ThisEvent);  
    // Update LastDelta.
    LastDelta = CurrentDelta;
    // Return True.
    return True;   
   } else
   {
      return False;
   }
      
}




boolean Check4BallsInPlay(void)
{
  static unsigned char LastPinState = 0;
  unsigned char CurrentPinState;
  boolean ReturnVal = False;

  CurrentPinState =  1; //LOCK_PIN;
  // check for pin high and different from last time
  if ( (CurrentPinState != LastPinState) &&
       (CurrentPinState == LOCK_PIN) )
  {                     // event detected, so post detected event
    ES_Event ThisEvent;
    ThisEvent.EventType = BALLS_IN_PLAY;
    ThisEvent.EventParam = 1;
    //ES_PostList02(ThisEvent); // this could be any SM post function or EF_PostAll
    //ReturnVal = True;
    ReturnVal = False;
  }
  LastPinState = CurrentPinState;

  return ReturnVal;
}

boolean Check4PositionFound(void)
{ 
  boolean ReturnVal = False;

  
  float CurrentXPosition = accessXPosition();
  float CurrentYPosition = accessYPosition();
  float CurrentBotOrientation = accessAngle();
  
  // check for pin high and different from last time
  
  if ( (CurrentXPosition != LastXPosition) || 
       (CurrentYPosition != LastYPosition) ||
       (CurrentBotOrientation != LastBotOrientation) )
  {
   ES_Event ThisEvent;
   ThisEvent.EventType = POSITION_FOUND;
   ES_PostList01(ThisEvent);
   ReturnVal = True;
   //ReturnVal = False;
  }
  

  LastXPosition = CurrentXPosition;
  LastYPosition = CurrentYPosition;
  LastBotOrientation = CurrentBotOrientation;

  return ReturnVal;
}

boolean BallChecker(void)
{
  unsigned int CurrentBallStatus;
  boolean ReturnVal = False;

  CurrentBallStatus =  queryFSR( INIT_BALLS );

  if ( (CurrentBallStatus != LastBallStatus) )
  {                     
    ES_Event ThisEvent;
    ThisEvent.EventType = BALLS_IN_PLAY;
	ThisEvent.EventParam = CurrentBallStatus;
    ES_PostList02(ThisEvent); 
    ReturnVal = True;
  }
  LastBallStatus = CurrentBallStatus;

  return ReturnVal;
}

boolean BinChecker(void)
{
  unsigned int CurrentBin1Status;
  unsigned int CurrentBin2Status;
  unsigned int CurrentBin3Status;
  unsigned int CurrentBin4Status;
  
  boolean ReturnVal = False;

  CurrentBin1Status =  queryFSR( BIN1 );
  CurrentBin2Status =  queryFSR( BIN2 );
  CurrentBin3Status =  queryFSR( BIN3 );
  CurrentBin4Status =  queryFSR( BIN4 );

  if ( (CurrentBin1Status != LastBin1Status)|
	   (CurrentBin2Status != LastBin2Status)|
	   (CurrentBin3Status != LastBin3Status)|
	   (CurrentBin4Status != LastBin4Status) )
  {                     
    ES_Event ThisEvent;
    ThisEvent.EventType = BIN_CHANGED;
    if (CurrentBin1Status != LastBin1Status)
    {
      ThisEvent.EventParam = 1;
    } else if(CurrentBin2Status != LastBin2Status)
    {
      ThisEvent.EventParam = 2;
    } else if(CurrentBin3Status != LastBin3Status)
    {
      ThisEvent.EventParam = 3;
    } else
    {
      ThisEvent.EventParam = 4;
    }
    ES_PostList01(ThisEvent); 
    ReturnVal = True;
  }
  LastBin1Status = CurrentBin1Status;
  LastBin2Status = CurrentBin2Status;
  LastBin3Status = CurrentBin3Status;
  LastBin4Status = CurrentBin4Status;

  return ReturnVal;
}

boolean WallAngleChecker(void)
{
  unsigned int CurrentWallAngleStatus;
  boolean ReturnVal = False;

  CurrentWallAngleStatus =  queryFSR( WALL_ANGLE );

  if ( (CurrentWallAngleStatus != LastWallAngleStatus) )
  {                     
    ES_Event ThisEvent;
    ThisEvent.EventType = WALL_MOVED;
    ThisEvent.EventParam = (CurrentWallAngleStatus - LastWallAngleStatus);
    ES_PostList01(ThisEvent); 
    ReturnVal = True;
  }
  LastWallAngleStatus = CurrentWallAngleStatus;

  return ReturnVal;
}

// Start Stepper and IR Event Checkers
/************************************/

boolean Beacon_Bin1_Checker(void)
{
  boolean ReturnVal = False;
  boolean CurrentHoneStatus;
  boolean CurrentRStatus;
  boolean CurrentLStatus;
  
  CurrentRStatus = Beacon_Bin1_R;
  CurrentLStatus = Beacon_Bin1_L;
  
   // This if statement prevents CurrentHoneStatus from being assigned
  // something that is inconsistent with CurrentRStatus and CurrentLStatus
  // This could happen if either CurrentRStatus or CurrentLStatus changed 
  // in between assignments.
  
  if (CurrentRStatus && CurrentLStatus){
	CurrentHoneStatus = True;
  } else
  {	
	CurrentHoneStatus = False;
  }


  if ( (CurrentHoneStatus != LastHoneStatus_Bin1) &&
       (CurrentHoneStatus) )
  {                    
    ES_Event ThisEvent;
	//printf("Event checker: BEACON_BIN1_HONED.\n\r");
    ThisEvent.EventType = BEACON_BIN1_HONED;
    ThisEvent.EventParam = accessStepCount(); // update (type concern)
    ES_PostList05(ThisEvent); // update with correct post list number
    ReturnVal = True;
  } else if ( (CurrentRStatus != LastRStatus_Bin1) &&
			  (CurrentRStatus) )
  {
	ES_Event ThisEvent;
	//printf("Event checker: BEACON_BIN1_RIGHT.\n\r");
    ThisEvent.EventType = BEACON_BIN1_RIGHT;  // as in right-only
    ThisEvent.EventParam = accessStepCount(); 
    ES_PostList05(ThisEvent); // update with correct post list number
    ReturnVal = True;
  } else if ( (CurrentLStatus != LastLStatus_Bin1) &&
			  (CurrentLStatus) )
  {
	ES_Event ThisEvent;
	//printf("Event checker: BEACON_BIN1_LEFT.\n\r");
    ThisEvent.EventType = BEACON_BIN1_LEFT;  // as in left-only
    ThisEvent.EventParam = accessStepCount();
    ES_PostList05(ThisEvent); // update with correct post list number
    ReturnVal = True;
  } else if ( (CurrentHoneStatus != LastHoneStatus_Bin1) &&
			  (CurrentLStatus) )
  {
	ES_Event ThisEvent;
	//printf("Event checker: BEACON_BIN1_RIGHT_LOST.\n\r");
    ThisEvent.EventType = BEACON_BIN1_RIGHT_LOST;  
    ThisEvent.EventParam = accessStepCount();
    ES_PostList05(ThisEvent); // update with correct post list number
    ReturnVal = True;
  }else if ( (CurrentHoneStatus != LastHoneStatus_Bin1) &&
			  (CurrentRStatus) )
  {
	ES_Event ThisEvent;
	//printf("Event checker: BEACON_BIN1_LEFT_LOST.\n\r");
    ThisEvent.EventType = BEACON_BIN1_LEFT_LOST;  
    ThisEvent.EventParam = accessStepCount();
    ES_PostList05(ThisEvent); // update with correct post list number
    ReturnVal = True;
  }

  LastHoneStatus_Bin1 = CurrentHoneStatus;
  LastRStatus_Bin1 = CurrentRStatus;
  LastLStatus_Bin1 = CurrentLStatus;

  return ReturnVal;
}

boolean Beacon_Bin2_Checker(void)
{
  boolean ReturnVal = False;
  boolean CurrentHoneStatus;
  boolean CurrentRStatus;
  boolean CurrentLStatus;
  
  CurrentRStatus = Beacon_Bin2_R;
  CurrentLStatus = Beacon_Bin2_L;
  
   // This if statement prevents CurrentHoneStatus from being assigned
  // something that is inconsistent with CurrentRStatus and CurrentLStatus
  // This could happen if either CurrentRStatus or CurrentLStatus changed 
  // in between assignments.
  
  if (CurrentRStatus && CurrentLStatus){
	CurrentHoneStatus = True;
  } else
  {	
	CurrentHoneStatus = False;
  }


  if ( (CurrentHoneStatus != LastHoneStatus_Bin2) &&
       (CurrentHoneStatus) )
  {                    
    ES_Event ThisEvent;
	//printf("Event checker: BEACON_BIN2_HONED.\n\r");
    ThisEvent.EventType = BEACON_BIN2_HONED;
    ThisEvent.EventParam = accessStepCount(); // update (type concern)
    ES_PostList05(ThisEvent); // update with correct post list number
    ReturnVal = True;
  } else if ( (CurrentRStatus != LastRStatus_Bin2) &&
			  (CurrentRStatus) )
  {
	ES_Event ThisEvent;
	//printf("Event checker: BEACON_BIN2_RIGHT.\n\r");
    ThisEvent.EventType = BEACON_BIN2_RIGHT;  // as in right-only
    ThisEvent.EventParam = accessStepCount(); 
    ES_PostList05(ThisEvent); // update with correct post list number
    ReturnVal = True;
  } else if ( (CurrentLStatus != LastLStatus_Bin2) &&
			  (CurrentLStatus) )
  {
	ES_Event ThisEvent;
	//printf("Event checker: BEACON_BIN2_LEFT.\n\r");
    ThisEvent.EventType = BEACON_BIN2_LEFT;  // as in left-only
    ThisEvent.EventParam = accessStepCount();
    ES_PostList05(ThisEvent); // update with correct post list number
    ReturnVal = True;
  }else if ( (CurrentHoneStatus != LastHoneStatus_Bin2) &&
			  (CurrentLStatus) )
  {
	ES_Event ThisEvent;
	//printf("Event checker: BEACON_BIN2_RIGHT_LOST.\n\r");
    ThisEvent.EventType = BEACON_BIN2_RIGHT_LOST;  
    ThisEvent.EventParam = accessStepCount();
    ES_PostList05(ThisEvent); // update with correct post list number
    ReturnVal = True;
  }else if ( (CurrentHoneStatus != LastHoneStatus_Bin2) &&
			  (CurrentRStatus) )
  {
	ES_Event ThisEvent;
	//printf("Event checker: BEACON_BIN2_LEFT_LOST.\n\r");
    ThisEvent.EventType = BEACON_BIN2_LEFT_LOST;  
    ThisEvent.EventParam = accessStepCount();
    ES_PostList05(ThisEvent); // update with correct post list number
    ReturnVal = True;
  }

  LastHoneStatus_Bin2 = CurrentHoneStatus;
  LastRStatus_Bin2 = CurrentRStatus;
  LastLStatus_Bin2 = CurrentLStatus;

  return ReturnVal;
}

boolean Beacon_Bin3_Checker(void)
{
  boolean ReturnVal = False;
  boolean CurrentHoneStatus;
  boolean CurrentRStatus;
  boolean CurrentLStatus;
  
  CurrentRStatus = Beacon_Bin3_R;
  CurrentLStatus = Beacon_Bin3_L;
  
   // This if statement prevents CurrentHoneStatus from being assigned
  // something that is inconsistent with CurrentRStatus and CurrentLStatus
  // This could happen if either CurrentRStatus or CurrentLStatus changed 
  // in between assignments.
  
  if (CurrentRStatus && CurrentLStatus){
	CurrentHoneStatus = True;
  } else
  {	
	CurrentHoneStatus = False;
  }


  if ( (CurrentHoneStatus != LastHoneStatus_Bin3) &&
       (CurrentHoneStatus) )
  {                    
    ES_Event ThisEvent;
	//printf("Event checker: BEACON_BIN3_HONED.\n\r");
    ThisEvent.EventType = BEACON_BIN3_HONED;
    ThisEvent.EventParam = accessStepCount(); // update (type concern)
    ES_PostList05(ThisEvent); // update with correct post list number
    ReturnVal = True;
  } else if ( (CurrentRStatus != LastRStatus_Bin3) &&
			  (CurrentRStatus) )
  {
	ES_Event ThisEvent;
	//printf("Event checker: BEACON_BIN3_RIGHT.\n\r");
    ThisEvent.EventType = BEACON_BIN3_RIGHT;  // as in right-only
    ThisEvent.EventParam = accessStepCount(); 
    ES_PostList05(ThisEvent); // update with correct post list number
    ReturnVal = True;
  } else if ( (CurrentLStatus != LastLStatus_Bin3) &&
			  (CurrentLStatus) )
  {
	ES_Event ThisEvent;
	//printf("Event checker: BEACON_BIN3_LEFT.\n\r");
    ThisEvent.EventType = BEACON_BIN3_LEFT;  // as in left-only
    ThisEvent.EventParam = accessStepCount();
    ES_PostList05(ThisEvent); // update with correct post list number
    ReturnVal = True;
  }else if ( (CurrentHoneStatus != LastHoneStatus_Bin3) &&
			  (CurrentLStatus) )
  {
	ES_Event ThisEvent;
	//printf("Event checker: BEACON_BIN3_RIGHT_LOST.\n\r");
    ThisEvent.EventType = BEACON_BIN3_RIGHT_LOST;  
    ThisEvent.EventParam = accessStepCount();
    ES_PostList05(ThisEvent); // update with correct post list number
    ReturnVal = True;
  }else if ( (CurrentHoneStatus != LastHoneStatus_Bin3) &&
			  (CurrentRStatus) )
  {
	ES_Event ThisEvent;
	//printf("Event checker: BEACON_BIN3_LEFT_LOST.\n\r");
    ThisEvent.EventType = BEACON_BIN3_LEFT_LOST;  
    ThisEvent.EventParam = accessStepCount();
    ES_PostList05(ThisEvent); // update with correct post list number
    ReturnVal = True;
  }

  LastHoneStatus_Bin3 = CurrentHoneStatus;
  LastRStatus_Bin3 = CurrentRStatus;
  LastLStatus_Bin3 = CurrentLStatus;

  return ReturnVal;
}

boolean Beacon_Bin4_Checker(void)
{
  boolean ReturnVal = False;
  boolean CurrentHoneStatus;
  boolean CurrentRStatus;
  boolean CurrentLStatus;
  
  CurrentRStatus = Beacon_Bin4_R;
  CurrentLStatus = Beacon_Bin4_L;
  
   // This if statement prevents CurrentHoneStatus from being assigned
  // something that is inconsistent with CurrentRStatus and CurrentLStatus
  // This could happen if either CurrentRStatus or CurrentLStatus changed 
  // in between assignments.
  
  if (CurrentRStatus && CurrentLStatus){
	CurrentHoneStatus = True;
  } else
  {	
	CurrentHoneStatus = False;
  }


  if ( (CurrentHoneStatus != LastHoneStatus_Bin4) &&
       (CurrentHoneStatus) )
  {                    
    ES_Event ThisEvent;
	//printf("Event checker: BEACON_BIN4_HONED.\n\r");
    ThisEvent.EventType = BEACON_BIN4_HONED;
    ThisEvent.EventParam = accessStepCount(); // update (type concern)
    ES_PostList05(ThisEvent); // update with correct post list number
    ReturnVal = True;
  } else if ( (CurrentRStatus != LastRStatus_Bin2) &&
			  (CurrentRStatus) )
  {
	ES_Event ThisEvent;
//printf("Event checker: BEACON_BIN4_RIGHT.\n\r");
    ThisEvent.EventType = BEACON_BIN4_RIGHT;  // as in right-only
    ThisEvent.EventParam = accessStepCount(); 
    ES_PostList05(ThisEvent); // update with correct post list number
    ReturnVal = True;
  } else if ( (CurrentLStatus != LastLStatus_Bin4) &&
			  (CurrentLStatus) )
  {
	ES_Event ThisEvent;
	//printf("Event checker: BEACON_BIN4_LEFT.\n\r");
    ThisEvent.EventType = BEACON_BIN4_LEFT;  // as in left-only
    ThisEvent.EventParam = accessStepCount();
    ES_PostList05(ThisEvent); // update with correct post list number
    ReturnVal = True;
  }else if ( (CurrentHoneStatus != LastHoneStatus_Bin4) &&
			  (CurrentLStatus) )
  {
	ES_Event ThisEvent;
	//printf("Event checker: BEACON_BIN4_RIGHT_LOST.\n\r");
    ThisEvent.EventType = BEACON_BIN4_RIGHT_LOST;  
    ThisEvent.EventParam = accessStepCount();
    ES_PostList05(ThisEvent); // update with correct post list number
    ReturnVal = True;
  }else if ( (CurrentHoneStatus != LastHoneStatus_Bin4) &&
			  (CurrentRStatus) )
  {
	ES_Event ThisEvent;
	//printf("Event checker: BEACON_BIN4_LEFT_LOST.\n\r");
    ThisEvent.EventType = BEACON_BIN4_LEFT_LOST;  
    ThisEvent.EventParam = accessStepCount();
    ES_PostList05(ThisEvent); // update with correct post list number
    ReturnVal = True;
  }

  LastHoneStatus_Bin4 = CurrentHoneStatus;
  LastRStatus_Bin4 = CurrentRStatus;
  LastLStatus_Bin4 = CurrentLStatus;

  return ReturnVal;
}

boolean AnyBeacons_Checker(void)
{
   boolean ReturnVal = False;
   boolean CurrentAnyBeacons =  LastRStatus_Bin1 |
								LastLStatus_Bin1 |
								LastRStatus_Bin2 |
								LastLStatus_Bin2 |
								LastRStatus_Bin3 |
								LastLStatus_Bin3 |
								LastRStatus_Bin4 |
								LastLStatus_Bin4;
								
   if ( (CurrentAnyBeacons != LastAnyBeacons) &&
		(CurrentAnyBeacons) )
   {
		ES_Event ThisEvent;
		printf("Event checker: NO_BEACON.\n\r");
		ThisEvent.EventType = NO_BEACON;
		ThisEvent.EventParam = accessStepCount();
		ES_PostList05(ThisEvent);
		ReturnVal = True;
	}
	
	LastAnyBeacons = CurrentAnyBeacons;
	
	return ReturnVal;
}

boolean Check4Tape(void)
{
  boolean CurrentTapeStatus;
  boolean ReturnVal = False;

  CurrentTapeStatus =  PTU & TAPE_PIN; 
  
  if ( (CurrentTapeStatus != LastTapeStatus) &&
       (CurrentTapeStatus != False) )
  { 
    ES_Event ThisEvent;
    printf("Event checker: ZERO_FOUND\n\r");
    
    ThisEvent.EventType = ZERO_FOUND;
    ThisEvent.EventParam = 1;
    ES_PostList06(ThisEvent);
    // if we're in calibration state over in TankSM?
    ES_PostList00(ThisEvent);
    
    ReturnVal = True;
  }
  LastTapeStatus = CurrentTapeStatus;

  return ReturnVal;
}

void InitInterrupts(void)
{
	TIM0_TSCR1 = _S12_TEN;							// enable timers
	TIM0_TSCR2 = _S12_PR0 | _S12_PR1 | _S12_PR2;	// prescale to 128
	TIM0_TCTL3 |= _S12_EDG4A;						// capture rising edges
	TIM0_TCTL3 |= _S12_EDG5A;
	TIM0_TIOS &= ~_S12_IOS4;
	TIM0_TIOS &= ~_S12_IOS5;
	TIM0_TCTL1 = TIM0_TCTL1 & ~(_S12_OL4 | _S12_OM4);	// no pins connected
	TIM0_TCTL1 = TIM0_TCTL1 & ~(_S12_OL5 | _S12_OM5);
	TIM0_TIE |= _S12_C4I;							// enable IC4 interrupt
	TIM0_TIE |= _S12_C5I;							// enable IC5 interrupt
	TIM0_TFLG1 = _S12_C4F;							// clear flags
	TIM0_TFLG1 = _S12_C5F;
	
	TIM0_TIOS |= _S12_IOS7;							// set IO line 7 to OC
	TIM0_TCTL1 = TIM0_TCTL1 & ~_S12_OM7;			// initially no pin connected
	TIM0_TCTL1 = TIM0_TCTL1 & ~_S12_OL7;		
	TIM0_TIE = TIM0_TIE & ~_S12_C7I;				// disable OC7 interrupt
	TIM0_TFLG1 = _S12_C7F;							// clear flag	
	
	EnableInterrupts;								// global enable
}

void interrupt _Vec_tim0ch4 IRInterruptL(void)
{	
	//In case we want the interrupt to post an event
	//ES_Event ThisEvent;
	
	// make period measurement
	uPeriodL = TIM0_TC4 - uLastPulseL;
	uLastPulseL = TIM0_TC4;	
	//EnableInterrupts;
	//printf("Measured period on left channel: %u \n\r", uPeriodL);
	
	// clear flag
	TIM0_TFLG1 = _S12_C4F;
	
	if ( (uPeriodL < (BeaconArray[0] + BEACON_ERROR) ) &&
		 (uPeriodL > (BeaconArray[0] - BEACON_ERROR) ) )
		{
			Beacon_Bin1_L = True;
			//printf("Found Bin 1 Beacon L\n\r");
			//ScanStepPositionArray[0] = CurrentStepCount; handle this in event checker
			//ScanStepCountArray[0]++;
			//ScanStepCountArray[1]=0;
			//ScanStepCountArray[2]=0;
			//ScanStepCountArray[3]=0;
			/*ThisEvent.EventType = BEACON_Bin1;
			ThisEvent.EventParam = uPeriod;
			ES_PostList07(ThisEvent);*/	
		}	
	else
		{
			Beacon_Bin1_L = False;
		}
		
		
	if ( (uPeriodL < (BeaconArray[1] + BEACON_ERROR) ) &&
		 (uPeriodL > (BeaconArray[1] - BEACON_ERROR) ) )
		{
			Beacon_Bin2_L = True;
			//printf("Found Bin 2 Beacon L\n\r");			

		}
	else
		{
			Beacon_Bin2_L = False;
		}
	if ( (uPeriodL < (BeaconArray[2] + BEACON_ERROR) ) &&
		 (uPeriodL > (BeaconArray[2] - BEACON_ERROR) ) )
		{
			Beacon_Bin3_L = True;
			//printf("Found Bin 3 Beacon L\n\r");				
		
		}
	else
		{
			Beacon_Bin3_L = False;
		}
	if ( (uPeriodL < (BeaconArray[3] + BEACON_ERROR) ) &&
		 (uPeriodL > (BeaconArray[3] - BEACON_ERROR) ) )
		{
			Beacon_Bin4_L = True;
			//printf("Found Bin 4 Beacon L\n\r");			
		}		
	else
		{
			Beacon_Bin4_L = False;
		}
}

void interrupt _Vec_tim0ch5 IRInterrupt(void)
{	
	//In case we want the interrupt to post an event
	//ES_Event ThisEvent;
	
	// make period measurement
	uPeriod = TIM0_TC5 - uLastPulse;
	uLastPulse = TIM0_TC5;	
	//EnableInterrupts;
	//printf("Measured period on right channel: %u \n\r", uPeriod);
	
	// clear flag
	TIM0_TFLG1 = _S12_C5F;
	
	if ( (uPeriod < (BeaconArray[0] + BEACON_ERROR) ) &&
		 (uPeriod > (BeaconArray[0] - BEACON_ERROR) ) )
		{
			Beacon_Bin1_R = True;
			//printf("Found Bin 1 Beacon\n\r");
			//ScanStepPositionArray[0] = CurrentStepCount; handle this in event checker
			//ScanStepCountArray[0]++;
			//ScanStepCountArray[1]=0;
			//ScanStepCountArray[2]=0;
			//ScanStepCountArray[3]=0;
			/*ThisEvent.EventType = BEACON_Bin1;
			ThisEvent.EventParam = uPeriod;
			ES_PostList07(ThisEvent);*/
			
		}	
	else
		{
			Beacon_Bin1_R = False;
		}
		
		
	if ( (uPeriod < (BeaconArray[1] + BEACON_ERROR) ) &&
		 (uPeriod > (BeaconArray[1] - BEACON_ERROR) ) )
		{
			Beacon_Bin2_R = True;
			//printf("Found Bin 2 Beacon\n\r");			

		}
	else
		{
			Beacon_Bin2_R = False;
		}
	if ( (uPeriod < (BeaconArray[2] + BEACON_ERROR) ) &&
		 (uPeriod > (BeaconArray[2] - BEACON_ERROR) ) )
		{
			Beacon_Bin3_R = True;
			//printf("Found Bin 3 Beacon\n\r");				
		
		}
	else
		{
			Beacon_Bin3_R = False;
		}
	if ( (uPeriod < (BeaconArray[3] + BEACON_ERROR) ) &&
		 (uPeriod > (BeaconArray[3] - BEACON_ERROR) ) )
		{
			Beacon_Bin4_R = True;
			//printf("Found Bin 4 Beacon\n\r");			
		}		
	else
		{
			Beacon_Bin4_R = False;
		}
		
		
}
