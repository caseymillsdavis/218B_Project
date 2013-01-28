/****************************************************************************
 Module
   TemplateService.c

 Revision
   1.0.1

 Description
   This is a template file for implementing a simple service under the 
   Gen2 Events and Services Framework.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 01/16/12 09:58 jec      began conversion from TemplateFSM.c
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "GameMonitorService.h"
#include "ES_ServiceHeaders.h"
#include "GameConstants.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "PathPlannerModule.h"
#include "ES_PostList.h"
#include "TreadDriveSM.h"

/*----------------------------- Module Defines ----------------------------*/

#define CLOCKWISE 1
#define COUNTERCLOCKWISE 0 

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/

float ConvertTimeToAngleRotated(unsigned int MeasuredTime);
float ConvertTimeToDistanceTraveled(unsigned int MeasuredTime);


/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

static unsigned int WallAngle;
static unsigned int NumberOfBallsInPlay;
static unsigned int BallsInBinOne;
static unsigned int BallsInBinTwo;
static unsigned int BallsInBinThree;
static unsigned int BallsInBinFour; 

static float CurrentX;
static float CurrentY;
static float CurrentTheta;
static float CurrentSummedX;
static float CurrentSummedY;
static float CurrentSummedTheta;

static boolean WallVelocity;

static boolean Driving;
static boolean Rotating;
static boolean CounterClockwise;
static boolean Averaging;

static unsigned int StartTime;
static float ExactGoalTime;
//static unsigned int GoalTime;

static float WallLine[2] = {0,48}; //{INFINITY,NO_INTERCEPT};

static float FirstPointForWallControl[2];
static float StagingPointForLeftWall[2];
static float StagingPointForRightWall[2];
static float WallPushPointOne[2];
static float WallPushPointTwo[2];


/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitTemplateService

 Parameters
     uint8_t : the priorty of this service

 Returns
     boolean, False if error in initialization, True otherwise

 Description
     Saves away the priority, and does any 
     other required initialization for this service
 Notes

 Author
     J. Edward Carryer, 01/16/12, 10:00
****************************************************************************/
boolean InitGameMonitorService ( uint8_t Priority )
{
  ES_Event ThisEvent;

  MyPriority = Priority;
  /********************************************
   in here you write your initialization code
   *******************************************/
  //ES_Timer_InitTimer(MOVEMENT_TIMER,DISPLAY_REFRESH_TIME);
  CurrentX = accessXPosition();
  CurrentY = accessYPosition();
  CurrentTheta = accessAngle();
  WallAngle = queryFSR( WALL_ANGLE );
  
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
     PostTemplateService

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
boolean PostGameMonitorService( ES_Event ThisEvent )
{
  return ES_PostToService( MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunTemplateService

 Parameters
   ES_Event : the event to process

 Returns                          
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   add your description here
 Notes
   
 Author
   J. Edward Carryer, 01/15/12, 15:23
****************************************************************************/
ES_Event RunGameMonitorService( ES_Event ThisEvent )
{
  ES_Event ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

  switch (ThisEvent.EventType)
  {
   case WALL_MOVED :

      WallAngle = WallAngle + ThisEvent.EventParam;     
      UpdateWallLine();
      UpdateWallPoints();
      
      
      if (ThisEvent.EventParam < 0 || ThisEvent.EventParam == 358)
      {
         WallVelocity = COUNTERCLOCKWISE;   
      } else if (ThisEvent.EventParam > 0 || ThisEvent.EventParam == -358)
      {
         WallVelocity = CLOCKWISE;
      }
      
   break;
   case BIN_CHANGED :
      switch (ThisEvent.EventParam)
      {
         case (1):
            BallsInBinOne++;
         break;
         
         case (2):
            BallsInBinTwo++;
         break;
         
         case (3):
            BallsInBinThree++;
         break;
         
         case (4):
            BallsInBinFour++;
         break;
         
      }
   break;
   case BALLS_IN_PLAY :
      NumberOfBallsInPlay = ThisEvent.EventParam;   
   break;
   /*
   case DRIVE_COMMAND :
      UpdateCurrentPosition();
      Driving = True;
      StartTime = ES_Timer_GetTime();
   break;
   case ROTATE_COMMAND :
      UpdateCurrentPosition();
      Rotating = True;
      StartTime = ES_Timer_GetTime(); 
   break;

   case HOLD_COMMAND :
      UpdateCurrentPosition();     
   break;
   */
   
   case GO_COMMAND :
      
      //UpdateCurrentPosition();
      Driving = True;      
      StartTime = ES_Timer_GetTime();
      
      
   
   break;
   case TURN_COMMAND :
      //UpdateCurrentPosition();
      Rotating = True;
      StartTime = ES_Timer_GetTime();    
      
      ExactGoalTime = CONVERT_ANGLE_TO_TIME*(/*(float)*/((signed int)(ThisEvent.EventParam)));
         
      if (ExactGoalTime > 0)
      {
         CounterClockwise = True;
         
      } else
      {
         CounterClockwise = False;   
      }
      
   break;

   case WAIT_COMMAND :
      {
      ES_Event ImmediateEvent;   
      
      //UpdateCurrentPosition();
      if (ThisEvent.EventParam == WAIT_FOR_POSITION_FOUND)
      {
         ImmediateEvent.EventType = FIND_POSITION;
         ImmediateEvent.EventParam = 1;
         ES_PostList05(ImmediateEvent);   
      } else if (ThisEvent.EventParam == WAIT_FOR_DEPOSIT_COMPLETE) 
      {
         //ImmediateEvent.EventType = DEPOSIT_COMMAND;
         //ImmediateEvent.EventParam = 1;
         //ES_PostList00(ImmediateEvent);
         //printf("NON-POSTED DEPOSIT\n\r");
      } else if (ThisEvent.EventParam == WAIT_FOR_A_TENTH_SECOND)
      {
         ES_Timer_InitTimer(GAME_MONITOR_TIMER,TENTH_OF_A_SECOND);   
      } else if (ThisEvent.EventParam == WAIT_FOR_A_WIGGLE_TIME)
      {
         ES_Timer_InitTimer(GAME_MONITOR_TIMER,WAIT_FOR_A_WIGGLE_TIME);
      } else
      {
         ES_Timer_InitTimer(GAME_MONITOR_TIMER,ThisEvent.EventParam);   
      }
      
      }
   break; 
	
   case POSITION_FOUND :
       {
       //int i;
       /*
       float TestX = accessXPosition();
       float TestY = accessYPosition();
       float TestTheta = accessAngle();
         
       if ((fabs(TestX - CurrentX)>5)||(fabs(TestY-CurrentY)>5))
       {
         ES_Event CheckEvent;
         CheckEvent.EventType = WAIT_COMMAND;
         CheckEvent.EventParam = WAIT_FOR_POSITION_FOUND;
         ES_PostList02(CheckEvent);
         //i++;
       } 
       */
       CurrentX = accessXPosition();
       CurrentY = accessYPosition();
       CurrentTheta = accessAngle();
	   //i=0;
		 
	      if (Averaging == True)
			{
			   CurrentSummedX = CurrentSummedX + accessXPosition();
			   CurrentSummedY = CurrentSummedY + accessYPosition();
			   CurrentSummedTheta = CurrentSummedTheta + accessAngle();
         }
       }
   break;
   
   case START_AVERAGING :
		{
		Averaging = True;
		}
	break;
	
	case STOP_AVERAGING :
		{
		Averaging = False;
		CurrentX = CurrentSummedX/2;
      CurrentY = CurrentSummedY/2;
      CurrentTheta = CurrentSummedTheta/2;
		CurrentSummedX = 0;
		CurrentSummedY = 0;
		CurrentSummedTheta = 0;
		}
	break;
   
   case ES_TIMEOUT :
      if (ThisEvent.EventParam == GAME_MONITOR_TIMER)
      {
         PathPlanner();   
      }
      //PathPlanner();
   break;
  }
  
  ReturnEvent.EventType = ES_NO_EVENT;
  return ReturnEvent;
}

/***************************************************************************
access functions
***************************************************************************/

void UpdateCurrentPosition(void)
{
   unsigned int StopTime = ES_Timer_GetTime();
   
   if (QueryTreadDriveSM() == DRIVING)
   {
      float DistanceTraveled = CONVERT_TIME_TO_DISTANCE*((float)(StopTime - StartTime));
      CurrentX = CurrentX + cosf(CurrentTheta DEG_TO_RAD)*DistanceTraveled;
      CurrentY = CurrentY + sinf(CurrentTheta DEG_TO_RAD)*DistanceTraveled;
      Driving = False;   
      //printf("measured time is %d, measured distance is %f\n\n\r",(StopTime-StartTime),DistanceTraveled);
   }
   
   if (QueryTreadDriveSM() == ROTATING)
   {
   float ChangeInAngle;   
      
      if (CounterClockwise == True)
      {
         ChangeInAngle = CONVERT_TIME_TO_ANGLE*((float)(StopTime - StartTime));   
      } else
      {
         ChangeInAngle = -1*CONVERT_TIME_TO_ANGLE*((float)(StopTime - StartTime));   
      }     
   //printf("I think I changed by %f degrees\n\r",ChangeInAngle);
   CurrentTheta = CurrentTheta + ChangeInAngle;
   Rotating = False;
   //printf("measured time is %d, measured angle is %f\n\n\r",(StopTime-StartTime),ChangeInAngle);
   }      
}

float GetWallSlope(void)
{
   return WallLine[0];
}

float GetWallIntercept(void)
{
   return WallLine[1];
}

float GetX(void)
{
   return CurrentX;   
}
float GetY(void)
{
   return CurrentY;
}
float GetTheta(void)
{
   return CurrentTheta;
}
unsigned int GetWallAngle(void)
{
   return WallAngle;   
}

float WallStagingPointX(boolean isRight)
{
   float WallX;
   if (isRight == True)
   {
   WallX = StagingPointForRightWall[0];   
   }else
   {
   WallX = StagingPointForLeftWall[0];   
   }
   return(WallX); 
}

float WallStagingPointY(boolean isRight)
{
   float WallY;
   if (isRight == True)
   {
   WallY = StagingPointForRightWall[1];   
   }else
   {
   WallY = StagingPointForLeftWall[1];   
   }
   return(WallY); 
}

float WallFirstPointX(void)
{
   float FirstX;
   FirstX = FirstPointForWallControl[0];
   return(FirstX); 
}

float WallFirstPointY(void)
{
   float FirstY;
   FirstY =  FirstPointForWallControl[1];
   return(FirstY); 
}

float PushPointX(void)
{
   float DistanceSquaredOne;
   float DistanceSquaredTwo;
   
   DistanceSquaredOne = (CurrentX-WallPushPointOne[0])*(CurrentX-WallPushPointOne[0])+(CurrentY-WallPushPointOne[1])*(CurrentY-WallPushPointOne[1]);
   DistanceSquaredTwo = (CurrentX-WallPushPointTwo[0])*(CurrentX-WallPushPointTwo[0])+(CurrentY-WallPushPointTwo[1])*(CurrentY-WallPushPointTwo[1]);
   
   if (DistanceSquaredOne < DistanceSquaredTwo)
   {
      return(WallPushPointOne[0]);
   } else
   {
      return(WallPushPointTwo[0]);
   } 
}
float PushPointY(void)
{
   float DistanceSquaredOne;
   float DistanceSquaredTwo;
   
   DistanceSquaredOne = (CurrentX-WallPushPointOne[0])*(CurrentX-WallPushPointOne[0])+(CurrentY-WallPushPointOne[1])*(CurrentY-WallPushPointOne[1]);
   DistanceSquaredTwo = (CurrentX-WallPushPointTwo[0])*(CurrentX-WallPushPointTwo[0])+(CurrentY-WallPushPointTwo[1])*(CurrentY-WallPushPointTwo[1]);
   
   
   if (DistanceSquaredOne < DistanceSquaredTwo)
   {
      return(WallPushPointOne[1]);
   } else
   {
      return(WallPushPointTwo[1]);
   }   
}

void UpdateWallPoints(void)
{
   float FirstCandidateOne[2];
   float FirstCandidateTwo[2];
   float LeftStageCandidateOne[2];
   float LeftStageCandidateTwo[2];
   float RightStageCandidateOne[2];
   float RightStageCandidateTwo[2];
   float LocalWallAngle = (float) RHR_CORRECT*WallAngle;
   float PerpendicularToWallAngleOne = (float) LocalWallAngle + 90.;
   float PerpendicularToWallAngleTwo = (float) LocalWallAngle - 90.;
   
   //float PerpendicularLine[] = { (tan(PerpendicularToWallAngle)),(48. - tan(PerpendicularToWallAngle)*48.)}

/************************************************************************
The following code generates six points: two that are located
along a line perpendicular to the wall, 36 inches from the wall:
these are known as FirstCandidateOne and FirstCandidateTwo.
The other four are points 36 inches along the wall and 12 inches away
from the wall on either side, and they are RightCandidates
and LeftCandidates.  The "Right" and "Left" comes from the idea
that if a robot was on the same side of the wall as a given "Right"
or "Left" candidate, and was located at the corresponding "First" point,
facing the wall, the points would be on its right or left according
to their names. After generating these points, the function uses our
knowledge of where the robot is to determine which three points are
on the same side of the wall as the robot, and returns those three
points.
*************************************************************************/

// First we deal with the simple case of a vertical wall (the wall starts flat in our coordinate frame)   
if((WallAngle == 90)||(WallAngle == 270 ) )
   {
      // first, update the wall push points - simple stuff.
      WallPushPointOne[0] = OUTER_RADIUS;
      WallPushPointOne[1] = OUTER_RADIUS+RADIUS_OF_SAFETY;
      
      WallPushPointTwo[0] = OUTER_RADIUS;
      WallPushPointTwo[1] = OUTER_RADIUS-RADIUS_OF_SAFETY;
      
      
      // Generate the two candidate points.
      FirstCandidateOne[0] = OUTER_RADIUS;
      FirstCandidateOne[1] = OUTER_RADIUS+ RADIUS_OF_SAFETY;
   
      FirstCandidateTwo[0] = OUTER_RADIUS;
      FirstCandidateTwo[1] = OUTER_RADIUS+ RADIUS_OF_SAFETY; 
      
      // If both FirstCandidateOne and the CurrentX are on the same side
      // of the center value, then keep Candidate One - otherwise it's Two.
      if ((FirstCandidateOne[0]>OUTER_RADIUS)==(CurrentX>OUTER_RADIUS))
      {
         FirstPointForWallControl[0] = FirstCandidateOne[0];
         FirstPointForWallControl[1] = FirstCandidateOne[1];   
      } else if ((FirstCandidateTwo[0]>OUTER_RADIUS)==(CurrentX>OUTER_RADIUS))
      {
         FirstPointForWallControl[0] = FirstCandidateTwo[0];
         FirstPointForWallControl[1] = FirstCandidateTwo[1]; 
      } else
      {
         //printf("epic maths fail in UpdateWallPoints!\n\r");
      }
      
      
      // Generate the two Left points: right 12 in, down 36 in, and 
      // left 12 in, up 36 in.
      LeftStageCandidateOne[0] = OUTER_RADIUS+RADIUS_OF_SAFETY;
      LeftStageCandidateOne[1] = OUTER_RADIUS-SAFE_DISTANCE_FROM_WALL;
      
      LeftStageCandidateTwo[0] = OUTER_RADIUS-RADIUS_OF_SAFETY;
      LeftStageCandidateTwo[1] = OUTER_RADIUS+SAFE_DISTANCE_FROM_WALL;
      
      // If both LeftCandidateOne and the CurrentX are on the same side
      // of the center value, then keep Candidate One - otherwise it's Two.
      if ((LeftStageCandidateOne[0]>OUTER_RADIUS)==(CurrentX>OUTER_RADIUS))
      {
         StagingPointForLeftWall[0] = LeftStageCandidateOne[0];
         StagingPointForLeftWall[1] = LeftStageCandidateOne[1];   
      } else if ((LeftStageCandidateTwo[0]>OUTER_RADIUS)==(CurrentX>OUTER_RADIUS))
      {
         StagingPointForLeftWall[0] = LeftStageCandidateTwo[0];
         StagingPointForLeftWall[1] = LeftStageCandidateTwo[1]; 
      } else
      {
         //printf("epic maths fail in UpdateWallPoints!\n\r");
      }
      
      
      // Generate the two Right points: right 12 in, up 36 in, and 
      // left 12 in, down 36 in.
      RightStageCandidateOne[0] = OUTER_RADIUS+RADIUS_OF_SAFETY;
      RightStageCandidateOne[1] = OUTER_RADIUS+SAFE_DISTANCE_FROM_WALL;
      
      RightStageCandidateTwo[0] = OUTER_RADIUS-RADIUS_OF_SAFETY;
      RightStageCandidateTwo[1] = OUTER_RADIUS-SAFE_DISTANCE_FROM_WALL;
      
      // If both RightCandidateOne and the CurrentX are on the same side
      // of the center value, then keep Candidate One - otherwise it's Two.
      if ((RightStageCandidateOne[0]>OUTER_RADIUS)==(CurrentX>OUTER_RADIUS))
      {
         StagingPointForRightWall[0] = RightStageCandidateOne[0];
         StagingPointForRightWall[1] = RightStageCandidateOne[1];   
      } else if ((LeftStageCandidateTwo[0]>OUTER_RADIUS)==(CurrentX>OUTER_RADIUS))
      {
         StagingPointForRightWall[0] = RightStageCandidateTwo[0];
         StagingPointForRightWall[1] = RightStageCandidateTwo[1]; 
      } else
      {
         //printf("epic maths fail in UpdateWallPoints!\n\r");
      }
          
// Next we deal with the simple case of a horizontal wall     
   } else if ((WallAngle == 0)||(WallAngle == 180 ) )
   {
      // first, update the wall push points - simple stuff.
      WallPushPointOne[0] = OUTER_RADIUS+RADIUS_OF_SAFETY;
      WallPushPointOne[1] = OUTER_RADIUS;
      
      WallPushPointTwo[0] = OUTER_RADIUS-RADIUS_OF_SAFETY;
      WallPushPointTwo[1] = OUTER_RADIUS;
      
      
      // Generate the two candidate points.
      FirstCandidateOne[0] = OUTER_RADIUS+RADIUS_OF_SAFETY;
      FirstCandidateOne[1] = OUTER_RADIUS;
      
      FirstCandidateTwo[0] = OUTER_RADIUS-RADIUS_OF_SAFETY;
      FirstCandidateTwo[1] = OUTER_RADIUS;
      
      // If both FirstCandidateOne and the CurrentY are on the same side
      // of 48, the center value, then keep Candidate One - otherwise it's Two.
      if ((FirstCandidateOne[1]>OUTER_RADIUS)==(CurrentY>OUTER_RADIUS))
      {
         FirstPointForWallControl[0] = FirstCandidateOne[0];
         FirstPointForWallControl[1] = FirstCandidateOne[1];   
      } else if ((FirstCandidateTwo[1]>OUTER_RADIUS)==(CurrentY>OUTER_RADIUS))
      {
         FirstPointForWallControl[0] = FirstCandidateTwo[0];
         FirstPointForWallControl[1] = FirstCandidateTwo[1]; 
      } else
      {
         //printf("epic maths fail in UpdateWallPoints!\n\r");
      }

      // Generate the two Left points: left 36 in, down 12 in, and 
      // right 36 in, up 12 in.
      LeftStageCandidateOne[0] = OUTER_RADIUS-RADIUS_OF_SAFETY;
      LeftStageCandidateOne[1] = OUTER_RADIUS-SAFE_DISTANCE_FROM_WALL;
      
      LeftStageCandidateTwo[0] = OUTER_RADIUS+RADIUS_OF_SAFETY;
      LeftStageCandidateTwo[1] = OUTER_RADIUS+SAFE_DISTANCE_FROM_WALL;
      
      // If both LeftCandidateOne and the CurrentY are on the same side
      // of 48, the center value, then keep Candidate One - otherwise it's Two.
      if ((LeftStageCandidateOne[0]>OUTER_RADIUS)==(CurrentX>OUTER_RADIUS))
      {
         StagingPointForLeftWall[0] = LeftStageCandidateOne[0];
         StagingPointForLeftWall[1] = LeftStageCandidateOne[1];   
      } else if ((LeftStageCandidateTwo[0]>OUTER_RADIUS)==(CurrentX>OUTER_RADIUS))
      {
         StagingPointForLeftWall[0] = LeftStageCandidateTwo[0];
         StagingPointForLeftWall[1] = LeftStageCandidateTwo[1]; 
      } else
      {
         //printf("epic maths fail in UpdateWallPoints!\n\r");
      }
      
      
      // Generate the two Right points: right 36 in, down 12 in, and 
      // left 36 in, up 12 in.
      RightStageCandidateOne[0] = OUTER_RADIUS+RADIUS_OF_SAFETY;
      RightStageCandidateOne[1] = OUTER_RADIUS-SAFE_DISTANCE_FROM_WALL;
      
      RightStageCandidateTwo[0] = OUTER_RADIUS-RADIUS_OF_SAFETY;
      RightStageCandidateTwo[1] = OUTER_RADIUS+SAFE_DISTANCE_FROM_WALL;
      
      // If both RightCandidateOne and the CurrentY are on the same side
      // of 48, the center value, then keep Candidate One - otherwise it's Two.
      if ((RightStageCandidateOne[0]>OUTER_RADIUS)==(CurrentX>OUTER_RADIUS))
      {
         StagingPointForRightWall[0] = RightStageCandidateOne[0];
         StagingPointForRightWall[1] = RightStageCandidateOne[1];   
      } else if ((LeftStageCandidateTwo[0]>OUTER_RADIUS)==(CurrentX>OUTER_RADIUS))
      {
         StagingPointForRightWall[0] = RightStageCandidateTwo[0];
         StagingPointForRightWall[1] = RightStageCandidateTwo[1]; 
      } else
      {
         //printf("epic maths fail in UpdateWallPoints!\n\r");
      }
      
// Finally we deal with a non-vertical, non-horizontal wall position,
   } else
   {
      // first, update the wall push points - simple stuff.
      WallPushPointOne[0] = OUTER_RADIUS+RADIUS_OF_SAFETY*cosf(LocalWallAngle);
      WallPushPointOne[1] = OUTER_RADIUS+RADIUS_OF_SAFETY*sinf(LocalWallAngle);
      
      WallPushPointTwo[0] = OUTER_RADIUS-RADIUS_OF_SAFETY*cosf(LocalWallAngle);
      WallPushPointTwo[1] = OUTER_RADIUS-RADIUS_OF_SAFETY*sinf(LocalWallAngle);
      
      
      //Generate first points - just 36 Wy> in units of Nx> and Ny>
      FirstCandidateOne[0] = OUTER_RADIUS+ RADIUS_OF_SAFETY*cosf(PerpendicularToWallAngleOne);
      FirstCandidateOne[1] = OUTER_RADIUS+ RADIUS_OF_SAFETY*sinf(PerpendicularToWallAngleOne);
   
      FirstCandidateTwo[0] = OUTER_RADIUS+ RADIUS_OF_SAFETY*cosf(PerpendicularToWallAngleTwo);
      FirstCandidateTwo[1] = OUTER_RADIUS+ RADIUS_OF_SAFETY*sinf(PerpendicularToWallAngleTwo); 
      // See if we're above or below the WallLine in the same manner that our position is above or below WallLine.
      // I think this would work for the horizontal case as well, except that the PerpendicularToWallAngleTwo calculations
      // might a bit get messy.
      if ((FirstCandidateOne[1]>(WallLine[0]*FirstCandidateOne[1] + WallLine[1]))==(CurrentY>(WallLine[0]*CurrentY + WallLine[1])))
      {
         FirstPointForWallControl[0] = FirstCandidateOne[0];
         FirstPointForWallControl[1] = FirstCandidateOne[1];   
      } else if ((FirstCandidateTwo[1]>(WallLine[0]*FirstCandidateTwo[1] + WallLine[1]))==(CurrentY>(WallLine[0]*CurrentY + WallLine[1])))
      {
         FirstPointForWallControl[0] = FirstCandidateTwo[0];
         FirstPointForWallControl[1] = FirstCandidateTwo[1]; 
      } else
      {
         //printf("epic maths fail in UpdateWallPoints!\n\r");
      }
      
      //Generate left points - combinations of 36 Wx> and 12 Wy> in units of Nx> and Ny>
      
      //Right One: -36 Wx> + 12Wy>
      LeftStageCandidateOne[0] = OUTER_RADIUS- RADIUS_OF_SAFETY*cosf(LocalWallAngle) + SAFE_DISTANCE_FROM_WALL*cosf(PerpendicularToWallAngleOne);
      LeftStageCandidateOne[1] = OUTER_RADIUS- RADIUS_OF_SAFETY*sinf(LocalWallAngle)+ SAFE_DISTANCE_FROM_WALL*sinf(PerpendicularToWallAngleOne);
      //Left Two: +36 Wx> - 12Wy> (negative sign is in the perpendicular angle variable)
      LeftStageCandidateTwo[0] = OUTER_RADIUS+ RADIUS_OF_SAFETY*cosf(LocalWallAngle) + SAFE_DISTANCE_FROM_WALL*cosf(PerpendicularToWallAngleTwo);
      LeftStageCandidateTwo[1] = OUTER_RADIUS+ RADIUS_OF_SAFETY*sinf(LocalWallAngle)+ SAFE_DISTANCE_FROM_WALL*sinf(PerpendicularToWallAngleTwo); 
      
      // See if we're above or below the WallLine in the same manner that our position is above or below WallLine.
      // I think this would work for the horizontal case as well, except that the PerpendicularToWallAngleTwo calculations
      // might get a bit messy.
      if ((LeftStageCandidateOne[1]>(WallLine[0]*LeftStageCandidateOne[1] + WallLine[1]))==(CurrentY>(WallLine[0]*CurrentY + WallLine[1])))
      {
         StagingPointForLeftWall[0] = LeftStageCandidateOne[0];
         StagingPointForLeftWall[1] = LeftStageCandidateOne[1];   
      } else if ((LeftStageCandidateTwo[1]>(WallLine[0]*LeftStageCandidateTwo[1] + WallLine[1]))==(CurrentY>(WallLine[0]*CurrentY + WallLine[1])))
      {
         StagingPointForLeftWall[0] = LeftStageCandidateTwo[0];
         StagingPointForLeftWall[1] = LeftStageCandidateTwo[1];  
      } else
      {
         //printf("epic maths fail in UpdateWallPoints!\n\r");
      }
      
      //Generate right points - combinations of 36 Wx> and 12 Wy> in units of Nx> and Ny>
      
      //Right One: 36 Wx> + 12Wy>
      RightStageCandidateOne[0] = OUTER_RADIUS+ RADIUS_OF_SAFETY*cosf(LocalWallAngle) + SAFE_DISTANCE_FROM_WALL*cosf(PerpendicularToWallAngleOne);
      RightStageCandidateOne[1] = OUTER_RADIUS+ RADIUS_OF_SAFETY*sinf(LocalWallAngle)+ SAFE_DISTANCE_FROM_WALL*sinf(PerpendicularToWallAngleOne);
      //Right Two: -36 Wx> - 12Wy>
      RightStageCandidateTwo[0] = OUTER_RADIUS- RADIUS_OF_SAFETY*cosf(LocalWallAngle) + SAFE_DISTANCE_FROM_WALL*cosf(PerpendicularToWallAngleTwo);
      RightStageCandidateTwo[1] = OUTER_RADIUS- RADIUS_OF_SAFETY*sinf(LocalWallAngle)+ SAFE_DISTANCE_FROM_WALL*sinf(PerpendicularToWallAngleTwo); 
      
      // See if we're above or below the WallLine in the same manner that our position is above or below WallLine.
      // I think this would work for the horizontal case as well, except that the PerpendicularToWallAngleTwo calculations
      // might a bit get messy.
      if ((RightStageCandidateOne[1]>(WallLine[0]*RightStageCandidateOne[1] + WallLine[1]))==(CurrentY>(WallLine[0]*CurrentY + WallLine[1])))
      {
         StagingPointForRightWall[0] = RightStageCandidateOne[0];
         StagingPointForRightWall[1] = RightStageCandidateOne[1];   
      } else if ((FirstCandidateTwo[1]>(WallLine[0]*FirstCandidateTwo[1] + WallLine[1]))==(CurrentY>(WallLine[0]*CurrentY + WallLine[1])))
      {
         StagingPointForRightWall[0] = RightStageCandidateTwo[0];
         StagingPointForRightWall[1] = RightStageCandidateTwo[1]; 
      } else
      {
         //printf("epic maths fail in UpdateWallPoints!\n\r");
      }
        
   }
    
}


void UpdateWallLine(void)
{
   float _m_;
   float _b_;
   if ((WallAngle == 90)||(WallAngle==270))
   {
      _m_ = INFINITY;
      _b_ = NO_INTERCEPT;
   } else
   {
      float WallAngleFloat = (float) WallAngle;
      WallAngleFloat = WallAngleFloat*PI/180.;
      _m_ = tan(RHR_CORRECT*WallAngleFloat);
      _b_ = 48. - _m_*48.;
   }
   WallLine[0] = _m_;
   WallLine[1] = _b_;
  
}
/***************************************************************************
 private functions
 ***************************************************************************/
float ConvertTimeToAngleRotated(unsigned int MeasuredTime)
{
   float ReturnAngle = (float)MeasuredTime*CONVERT_TIME_TO_ANGLE;
   return (ReturnAngle);   
}
float ConvertTimeToDistanceTraveled(unsigned int MeasuredTime)
{
   float ReturnDistance = (float)MeasuredTime*CONVERT_TIME_TO_DISTANCE;
   return (ReturnDistance);     
}


/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/

