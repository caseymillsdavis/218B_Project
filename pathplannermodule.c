#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_PostList.h"
#include "ES_Timers.h"


#include "PathPlannerModule.h"
 
//#include "PositionControlSM.h"
#include "GameControlSM.h"
#include "ES_ServiceHeaders.h"
#include "GameConstants.h"
#include "MotorDrivers.h"
#include "TankSM.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#define X_COORDINATE 0
#define Y_COORDINATE 1
#define THETA 2


//Module function declarations
static ES_Event GenerateNextStep_GoalPoint(float CurrentPosition[3], float GoalOne[2], int EventParameter);
static ES_Event GenerateNextStep_RadialPath(float CurrentPosition[3],float FinalDistanceFromCenter,int EventParameter);  // for deposit state
static ES_Event GenerateNextStep_EscapeWallPath(float CurrentPositon[3],float WallAngle);
/************/

//Module variables
static int WallCheckPoints;

static int BinCheckPoints;


static int wigglecounter;

void PathPlanner(void)
{
if(QueryTankSM()!=DISABLED)
{

   static float CurrentPosition[3] = {0,0,0}; 
   static float CurrentWallLine[2];
   static int CurrentWallAngle;
   
   static ActivityState_t CurrentGoal;
   ES_Event ThinkTankEvent;
   ES_Event GameMonitorEvent;
   
   srand(ES_Timer_GetTime());
   
   if( CurrentGoal != QueryActivityControlSM())
   {
      //printf("clearing checkpoints\n\r");
     
	  wigglecounter = 0;
	  BinCheckPoints = 0;
	  WallCheckPoints = 0;
      
      
      // GOOD?
      ThinkTankEvent.EventType = WAIT_COMMAND;
      ThinkTankEvent.EventParam = WAIT_FOR_POSITION_FOUND;
      CurrentGoal = QueryActivityControlSM();
            
   } else
   {

   
   //ES_Event TankEvent;
   
   //CurrentGoal = QueryActivityControlSM();
   CurrentWallLine[0]= GetWallSlope(); 
   CurrentWallLine[1]= GetWallIntercept();
   CurrentWallAngle = GetWallAngle();
   UpdateCurrentPosition();
   CurrentPosition[X_COORDINATE] = GetX();
   CurrentPosition[Y_COORDINATE] = GetY();
   CurrentPosition[THETA] = GetTheta();
   //UpdateWallPoints(); // before getting staging points for wall
   
    switch (CurrentGoal)
   {
   //ES_Event ActionEvent;
      
      case RASTER :
      {
         static char i;
         
         //ThinkTankEvent.EventType = GO_COMMAND;
         //ThinkTankEvent.EventParam = 100;
         
          //Calibration test code
         /*
         if (i < 5)
         {
            ThinkTankEvent.EventType = GO_COMMAND;
            ThinkTankEvent.EventParam = STEP_DISTANCE; 
            i++;  
         } else if (i<6)
         {
            ThinkTankEvent.EventType = TURN_COMMAND;
            ThinkTankEvent.EventParam = 1080; 
            i++;  
         } else if (i<7)
         {
            ThinkTankEvent.EventType = TURN_COMMAND;
            ThinkTankEvent.EventParam = -1080; 
            i++;
         } else
         {
            ThinkTankEvent.EventType = WAIT_COMMAND;
            ThinkTankEvent.EventParam = WAIT_FOR_POSITION_FOUND; 
            i=0;
         }
         */ 
  
         
         // go forward thrice, turn randomly once, unless threatened by wall, then flee for safety.
         
         if (CurrentLocationIsSafe( CurrentPosition[0], CurrentPosition[1], CurrentPosition[2], CurrentWallAngle) == True)  //our current location is a safe one
         {
            
            if (i < 3)
            {
               if (SafeToTravel((float)STEP_DISTANCE/100.,CurrentPosition[0],CurrentPosition[1],CurrentPosition[2], CurrentWallAngle) == True)
               {
                  ThinkTankEvent.EventType = GO_COMMAND;
                  ThinkTankEvent.EventParam = STEP_DISTANCE;
                  
                  //SetMotorTimer(STEP_DISTANCE);
               } else
               {
                  ThinkTankEvent.EventType = TURN_COMMAND;
                  ThinkTankEvent.EventParam = (rand()%360) - 180; 
                  //SetMotorTimer(angle);
               }
               i++; 
            } else if (i<4)
            {
              ThinkTankEvent.EventType = WAIT_COMMAND;
              ThinkTankEvent.EventParam = WAIT_FOR_POSITION_FOUND;
              i++; 
            } else
            {
               ThinkTankEvent.EventType = TURN_COMMAND;
               ThinkTankEvent.EventParam = (rand()%360) - 180; 
               //SetMotorTimer(angle); 
               i = 0;    
            }
         } else
         {
            //go to safe point
            ThinkTankEvent = GenerateNextStep_EscapeWallPath(CurrentPosition,CurrentWallAngle);
         }
         
         
      }
      break;
      
      case CONTROL_LEFT_WALL :
         //printf("Wall Third Point is x: %d y: %d \n\r", PushPointX(),PushPointY());
         /*
         if(GetSeparationDistance( -- parameters  for push point -- ) < 6)
         {
            WallCheckPoints = 2;
         }
         */
         if (WallCheckPoints < 1)
         {
            float GoalPoint[2];
            //float NextGoalPoint[2];
            GoalPoint[0] = WallFirstPointX();
            GoalPoint[1] = WallFirstPointY();
            //NextGoalPoint[0] = WallStagingPointX(LEFT);
            //NextGoalPoint[1] = WallStagingPointY(LEFT);
            
            ThinkTankEvent = GenerateNextStep_GoalPoint(CurrentPosition,GoalPoint,WAIT_FOR_POSITION_FOUND);   


            
         } else if (WallCheckPoints < 2)
         {
            float GoalPoint[2];
            //float NextGoalPoint[2];
            GoalPoint[0] = WallStagingPointX(LEFT);
            GoalPoint[1] = WallStagingPointY(LEFT);
            //NextGoalPoint[0] = PushPointX();
            //NextGoalPoint[1] = PushPointY();
            
            ThinkTankEvent = GenerateNextStep_GoalPoint(CurrentPosition,GoalPoint,WAIT_FOR_POSITION_FOUND); 
         } else if  (WallCheckPoints < 3)
         {
            float GoalPoint[2];
            //float NextGoalPoint[2];
            GoalPoint[0] = PushPointX();
            GoalPoint[1] = PushPointY();
            // change this!!
            //NextGoalPoint[0] = PushPointX();
            //NextGoalPoint[1] = PushPointY();
            
            ThinkTankEvent = GenerateNextStep_GoalPoint(CurrentPosition,GoalPoint,WAIT_FOR_POSITION_FOUND);
         }  else
         {
            // THINK THROUGH THIS
            ThinkTankEvent.EventType = WAIT_COMMAND;
            ThinkTankEvent.EventParam = WAIT_FOR_A_TENTH_SECOND;
            //CurrentGoal = STAY_PUT (will start process over again)
            //WallCheckPointThree = False;
         }
         
      break;
      
      case CONTROL_RIGHT_WALL :
         //printf("Wall Third Point is x: %d y: %d \n\r", PushPointX(),PushPointY());
         /*
         if(GetSeparationDistance( -- parameters  for staging point -- ) < 8)
         {
            WallCheckPoints = 2;
         }
         */
         if (WallCheckPoints < 1)
         {
            float GoalPoint[2];
            //float NextGoalPoint[2];
            GoalPoint[0] = WallFirstPointX();
            GoalPoint[1] = WallFirstPointY();
            //NextGoalPoint[0] = WallStagingPointX(LEFT);
            //NextGoalPoint[1] = WallStagingPointY(LEFT);
            
            ThinkTankEvent = GenerateNextStep_GoalPoint(CurrentPosition,GoalPoint,WAIT_FOR_POSITION_FOUND);   


            
         } else if (WallCheckPoints < 2)
         {
            float GoalPoint[2];
            //float NextGoalPoint[2];
            GoalPoint[0] = WallStagingPointX(RIGHT);
            GoalPoint[1] = WallStagingPointY(RIGHT);
            //NextGoalPoint[0] = PushPointX();
            //NextGoalPoint[1] = PushPointY();
            
            ThinkTankEvent = GenerateNextStep_GoalPoint(CurrentPosition,GoalPoint,WAIT_FOR_POSITION_FOUND); 
         } else if  (WallCheckPoints < 3)
         {
            float GoalPoint[2];
            //float NextGoalPoint[2];
            GoalPoint[0] = PushPointX();
            GoalPoint[1] = PushPointY();
            // change this!!
            //NextGoalPoint[0] = PushPointX();
            //NextGoalPoint[1] = PushPointY();
            
            ThinkTankEvent = GenerateNextStep_GoalPoint(CurrentPosition,GoalPoint,WAIT_FOR_POSITION_FOUND);
         }  else
         {
            // THINK THROUGH THIS
            ThinkTankEvent.EventType = WAIT_COMMAND;
            ThinkTankEvent.EventParam = WAIT_FOR_A_TENTH_SECOND;
            //CurrentGoal = STAY_PUT (will start process over again)
            //WallCheckPointThree = False;
         }
         
      break;
      
      
      case DEPOSIT_IN_BIN_ONE :
         
         if (BinCheckPoints < 1)
         {
			ThinkTankEvent = GenerateNextStep_RadialPath(CurrentPosition,RADIUS_OF_SAFETY-2*HALF_BOT_LENGTH,WAIT_FOR_POSITION_FOUND);   
            
         } else if (BinCheckPoints < 2)
         {
            float GoalPoint[2];
            //float NextGoalPoint[2];
            GoalPoint[0] = StagingPointForBinOneX;
            GoalPoint[1] = StagingPointForBinOneY;
            //NextGoalPoint[0] = BinOneDepositX;
            //NextGoalPoint[1] = BinOneDepositY;
            GameMonitorEvent.EventType = START_AVERAGING;
			ThinkTankEvent = GenerateNextStep_GoalPoint(CurrentPosition,GoalPoint,WAIT_FOR_POSITION_FOUND); 
         } else if (BinCheckPoints < 3)
		 {
			ThinkTankEvent.EventType = WAIT_COMMAND;
			ThinkTankEvent.EventParam = WAIT_FOR_POSITION_FOUND; 
			BinCheckPoints++;
		} else if (BinCheckPoints < 4)
		 {
			ThinkTankEvent.EventType = WAIT_COMMAND;
			ThinkTankEvent.EventParam = WAIT_FOR_POSITION_FOUND; 
			BinCheckPoints++;
		} else if (BinCheckPoints < 5)
         {
            
			GameMonitorEvent.EventType = STOP_AVERAGING;
			
			ThinkTankEvent.EventType = WAIT_COMMAND;
			ThinkTankEvent.EventParam = WAIT_FOR_A_TENTH_SECOND;
			
			BinCheckPoints++;
		} else if (BinCheckPoints < 6) 
		{
			float GoalPoint[2];
            //float NextGoalPoint[2];
            GoalPoint[0] = DepositPointForBinOneX;
            GoalPoint[1] = DepositPointForBinOneY;
			//UpdateCurrentPosition();
			//CurrentPosition[X_COORDINATE] = GetX();
			//CurrentPosition[Y_COORDINATE] = GetY();
			//CurrentPosition[THETA] = GetTheta();
         ThinkTankEvent =  GenerateNextStep_GoalPoint(CurrentPosition,GoalPoint,WAIT_FOR_A_TENTH_SECOND);    /* checkpoint */     
         }  else
         {
            ThinkTankEvent.EventType = WAIT_COMMAND;
            ThinkTankEvent.EventParam = WAIT_FOR_A_TENTH_SECOND;
            Drive(GENTLE_FORWARD);
         }
      break;
      
      case DEPOSIT_IN_BIN_TWO :
         
         if (BinCheckPoints < 1)
         {
			ThinkTankEvent = GenerateNextStep_RadialPath(CurrentPosition,RADIUS_OF_SAFETY-2*HALF_BOT_LENGTH,WAIT_FOR_POSITION_FOUND);   
            
         } else if (BinCheckPoints < 2)
         {
            float GoalPoint[2];
            //float NextGoalPoint[2];
            GoalPoint[0] = StagingPointForBinTwoX;
            GoalPoint[1] = StagingPointForBinTwoY;
            //NextGoalPoint[0] = BinOneDepositX;
            //NextGoalPoint[1] = BinOneDepositY;
            GameMonitorEvent.EventType = START_AVERAGING;
			ThinkTankEvent = GenerateNextStep_GoalPoint(CurrentPosition,GoalPoint,WAIT_FOR_POSITION_FOUND); 
         } else if (BinCheckPoints < 3)
		 {
			ThinkTankEvent.EventType = WAIT_COMMAND;
			ThinkTankEvent.EventParam = WAIT_FOR_POSITION_FOUND; 
			BinCheckPoints++;
		} else if (BinCheckPoints < 4)
		 {
			ThinkTankEvent.EventType = WAIT_COMMAND;
			ThinkTankEvent.EventParam = WAIT_FOR_POSITION_FOUND; 
			BinCheckPoints++;
		} else if (BinCheckPoints < 5)
         {
            
			GameMonitorEvent.EventType = STOP_AVERAGING;
			
			ThinkTankEvent.EventType = WAIT_COMMAND;
			ThinkTankEvent.EventParam = WAIT_FOR_A_TENTH_SECOND;
			
			BinCheckPoints++;
		} else if (BinCheckPoints < 6) 
		{
			float GoalPoint[2];
            //float NextGoalPoint[2];
            GoalPoint[0] = DepositPointForBinTwoX;
            GoalPoint[1] = DepositPointForBinTwoY;
			//UpdateCurrentPosition();
			//CurrentPosition[X_COORDINATE] = GetX();
			//CurrentPosition[Y_COORDINATE] = GetY();
			//CurrentPosition[THETA] = GetTheta();
         ThinkTankEvent =  GenerateNextStep_GoalPoint(CurrentPosition,GoalPoint,WAIT_FOR_A_TENTH_SECOND);    /* checkpoint */     
         }  else
         {
            ThinkTankEvent.EventType = WAIT_COMMAND;
            ThinkTankEvent.EventParam = WAIT_FOR_A_TENTH_SECOND;
            Drive(GENTLE_FORWARD);
         }
      break;
      
      
      case DEPOSIT_IN_BIN_THREE :
         if (BinCheckPoints < 1)
         {
			ThinkTankEvent = GenerateNextStep_RadialPath(CurrentPosition,RADIUS_OF_SAFETY-2*HALF_BOT_LENGTH,WAIT_FOR_POSITION_FOUND);   
            
         } else if (BinCheckPoints < 2)
         {
            float GoalPoint[2];
            //float NextGoalPoint[2];
            GoalPoint[0] = StagingPointForBinThreeX;
            GoalPoint[1] = StagingPointForBinThreeY;
            //NextGoalPoint[0] = BinOneDepositX;
            //NextGoalPoint[1] = BinOneDepositY;
            GameMonitorEvent.EventType = START_AVERAGING;
			ThinkTankEvent = GenerateNextStep_GoalPoint(CurrentPosition,GoalPoint,WAIT_FOR_POSITION_FOUND); 
         } else if (BinCheckPoints < 3)
		 {
			ThinkTankEvent.EventType = WAIT_COMMAND;
			ThinkTankEvent.EventParam = WAIT_FOR_POSITION_FOUND; 
			BinCheckPoints++;
		} else if (BinCheckPoints < 4)
		 {
			ThinkTankEvent.EventType = WAIT_COMMAND;
			ThinkTankEvent.EventParam = WAIT_FOR_POSITION_FOUND; 
			BinCheckPoints++;
		} else if (BinCheckPoints < 5)
         {
            
			GameMonitorEvent.EventType = STOP_AVERAGING;
			
			ThinkTankEvent.EventType = WAIT_COMMAND;
			ThinkTankEvent.EventParam = WAIT_FOR_A_TENTH_SECOND;
			
			BinCheckPoints++;
		} else if (BinCheckPoints < 6)
		{
		   float GoalPoint[2];
         //float NextGoalPoint[2];
         GoalPoint[0] = DepositPointForBinThreeX;
         GoalPoint[1] = DepositPointForBinThreeY;

		   //UpdateCurrentPosition();
			//CurrentPosition[X_COORDINATE] = GetX();
			//CurrentPosition[Y_COORDINATE] = GetY();
			//CurrentPosition[THETA] = GetTheta();
			ThinkTankEvent =  GenerateNextStep_GoalPoint(CurrentPosition,GoalPoint,WAIT_FOR_A_TENTH_SECOND);    /* checkpoint */     
         }  else if (BinCheckPoints < 7)
         {
            
         
            ThinkTankEvent.EventType = WAIT_COMMAND;
            ThinkTankEvent.EventParam = WAIT_FOR_A_TENTH_SECOND;
            Drive(GENTLE_FORWARD);
            wigglecounter++;
            
            if (30 < wigglecounter)
            {
               ES_Event DummyEvent;
               DummyEvent.EventType = DEPOSIT_COMMAND;
               DummyEvent.EventParam = 1;
               ES_PostList00(DummyEvent);
               //printf("dpt tmr end - DUMP\n\r");
               
               
               BinCheckPoints++;
               wigglecounter = 0;
            }
            
         } else
         {
            static int done_wiggling;
            
            ThinkTankEvent.EventType = WAIT_COMMAND;
            ThinkTankEvent.EventParam = WAIT_FOR_A_WIGGLE_TIME;
            
            
               
            if(done_wiggling < 22)   
            {
               if (wigglecounter == 0)
               {
                  float param = 50;
                  Drive(0);
                  Rotate(param);//*GENTLE_FORWARD);  
                  puts("-"); 
                  ThinkTankEvent.EventParam = 3*WAIT_FOR_A_TENTH_SECOND;  // 4?
               } else if (wigglecounter == 1)
               {
                  float param = -75;
                  puts("+");
                  Drive(0);
                  Rotate(param);//GENTLE_FORWARD);      
               } else if(wigglecounter == 2) 
               {
                  float param = 50;
                  Drive(0);
                  Rotate(param);//*GENTLE_FORWARD);  
                  puts("-"); 
                  ThinkTankEvent.EventParam = 3*WAIT_FOR_A_TENTH_SECOND;
               } else if(wigglecounter == 3)
               {
                  float param = -75;
                  puts("+");
                  Drive(0);
                  Rotate(param);//GENTLE_FORWARD);
                  wigglecounter = 0;
               }
            
            } else if (done_wiggling == 6)
            {
              
               ES_Event DummyEvent;
                Drive(0);
               DummyEvent.EventType = DEPOSIT_COMPLETE;
               DummyEvent.EventParam = 1;
               //ES_PostList02(DummyEvent);   
            }
            
            wigglecounter++;
            done_wiggling++;
         }
      break;
      
      case DEPOSIT_IN_BIN_FOUR :
         if (BinCheckPoints < 1)
         {
			ThinkTankEvent = GenerateNextStep_RadialPath(CurrentPosition,RADIUS_OF_SAFETY-2*HALF_BOT_LENGTH,WAIT_FOR_POSITION_FOUND);   
            
         } else if (BinCheckPoints < 2)
         {
            float GoalPoint[2];
            //float NextGoalPoint[2];
            GoalPoint[0] = StagingPointForBinFourX;
            GoalPoint[1] = StagingPointForBinFourY;
            //NextGoalPoint[0] = BinOneDepositX;
            //NextGoalPoint[1] = BinOneDepositY;
            GameMonitorEvent.EventType = START_AVERAGING;
			ThinkTankEvent = GenerateNextStep_GoalPoint(CurrentPosition,GoalPoint,WAIT_FOR_POSITION_FOUND); 
         } else if (BinCheckPoints < 3)
		 {
			ThinkTankEvent.EventType = WAIT_COMMAND;
			ThinkTankEvent.EventParam = WAIT_FOR_POSITION_FOUND; 
			BinCheckPoints++;
		} else if (BinCheckPoints < 4)
		 {
			ThinkTankEvent.EventType = WAIT_COMMAND;
			ThinkTankEvent.EventParam = WAIT_FOR_POSITION_FOUND; 
			BinCheckPoints++;
		} else if (BinCheckPoints < 5)
         {
            
			GameMonitorEvent.EventType = STOP_AVERAGING;
			
			ThinkTankEvent.EventType = WAIT_COMMAND;
			ThinkTankEvent.EventParam = WAIT_FOR_A_TENTH_SECOND;
			
			BinCheckPoints++;
		} else if (BinCheckPoints < 6)
		{
		   float GoalPoint[2];
         //float NextGoalPoint[2];
         GoalPoint[0] = DepositPointForBinFourX;
         GoalPoint[1] = DepositPointForBinFourY;

		   //UpdateCurrentPosition();
			//CurrentPosition[X_COORDINATE] = GetX();
			//CurrentPosition[Y_COORDINATE] = GetY();
			//CurrentPosition[THETA] = GetTheta();
			ThinkTankEvent =  GenerateNextStep_GoalPoint(CurrentPosition,GoalPoint,WAIT_FOR_A_TENTH_SECOND);    /* checkpoint */     
         }  else if (BinCheckPoints < 7)
         {
            
         
            ThinkTankEvent.EventType = WAIT_COMMAND;
            ThinkTankEvent.EventParam = WAIT_FOR_A_TENTH_SECOND;
            Drive(GENTLE_FORWARD);
            wigglecounter++;
            
            if (30 < wigglecounter)
            {
               ES_Event DummyEvent;
               DummyEvent.EventType = DEPOSIT_COMMAND;
               DummyEvent.EventParam = 1;
               ES_PostList00(DummyEvent);
               //printf("dpt tmr end - DUMP\n\r");
               
               
               BinCheckPoints++;
               wigglecounter = 0;
            }
            
         } else
         {
            static int done_wiggling;
            
            ThinkTankEvent.EventType = WAIT_COMMAND;
            ThinkTankEvent.EventParam = WAIT_FOR_A_WIGGLE_TIME;
            
            
               
            if(done_wiggling < 22)   
            {
               if (wigglecounter == 0)
               {
                  float param = 50;
                  Drive(0);
                  Rotate(param);//*GENTLE_FORWARD);  
                  puts("-"); 
                  ThinkTankEvent.EventParam = 3*WAIT_FOR_A_TENTH_SECOND;  // 4?
               } else if (wigglecounter == 1)
               {
                  float param = -75;
                  puts("+");
                  Drive(0);
                  Rotate(param);//GENTLE_FORWARD);      
               } else if(wigglecounter == 2) 
               {
                  float param = 50;
                  Drive(0);
                  Rotate(param);//*GENTLE_FORWARD);  
                  puts("-"); 
                  ThinkTankEvent.EventParam = 3*WAIT_FOR_A_TENTH_SECOND;
               } else if(wigglecounter == 3)
               {
                  float param = -75;
                  puts("+");
                  Drive(0);
                  Rotate(param);//GENTLE_FORWARD);
                  wigglecounter = 0;
               }
            
            } else if (done_wiggling == 6)
            {
              
               ES_Event DummyEvent;
                Drive(0);
               DummyEvent.EventType = DEPOSIT_COMPLETE;
               DummyEvent.EventParam = 1;
               //ES_PostList02(DummyEvent);   
            }
            
            wigglecounter++;
            done_wiggling++;
         }
      break;
      
     
      
      
      
   }
  
  
   }
/*  
   switch(ThinkTankEvent.EventType)
{
   case(TURN_COMMAND)   :
   printf("Current Event is TURN_COMMAND\n\r");
   break;
   case(GO_COMMAND)  :
   printf("Current Event is GO_COMMAND\n\r");
   break;
   case(WAIT_COMMAND)  :
   printf("Current Event is WAIT_COMMAND\n\r");
   break;
   case(ARRIVED) :
   printf("Current Event is whimsical.\n\r");
   default:
   printf("epic fail\n\r");
   
   
}
*/

if(QueryTankSM()!=DISABLED)

   ES_PostList00(ThinkTankEvent);
   ES_PostList04(ThinkTankEvent);
   ES_PostList03(GameMonitorEvent);
   printf("We're at X = %f, Y= %f, and Theta = %f, Wall is at %d\n\r", CurrentPosition[0],CurrentPosition[1],CurrentPosition[2],CurrentWallAngle);

   //ES_PostList02(TankEvent);
   //printf("event param = %f",ThinkTankEvent.EventParam);
}
}

boolean CurrentLocationIsSafe(float CurrentX, float CurrentY, float CurrentTHETA, int WallAngle)
{
   float CurrentPosition[3];
   float RadialDistanceSquared;
   float DistanceToWallSquared;
   float VectorToCenter[2];
   float UnitWallVector[2];
   
   CurrentPosition[0] = CurrentX;
   CurrentPosition[1] = CurrentY;
   CurrentPosition[2] = CurrentTHETA;
   
   RadialDistanceSquared = (CurrentPosition[0] - OUTER_RADIUS)*(CurrentPosition[0] - OUTER_RADIUS)
                            +(CurrentPosition[1] - OUTER_RADIUS)*(CurrentPosition[1] - OUTER_RADIUS);
     
   if (RadialDistanceSquared > ( RADIUS_OF_SAFETY*RADIUS_OF_SAFETY ) )
   {
      return False;
   } else
   {
      if (WallAngle == 0 || WallAngle == 180)
      {
         if ((CurrentPosition[1] < (OUTER_RADIUS + SAFE_DISTANCE_FROM_WALL))
               &&(CurrentPosition[1] > (OUTER_RADIUS - SAFE_DISTANCE_FROM_WALL)))
         {
            return False;
         } else
         {
            return True;
         }
      } else if (WallAngle == 90 || WallAngle == 270)
      {
         if ((CurrentPosition[0] < (OUTER_RADIUS + SAFE_DISTANCE_FROM_WALL))
               &&(CurrentPosition[0] > (OUTER_RADIUS - SAFE_DISTANCE_FROM_WALL)))
         {
            return False;
         } else
         {
            return True;
         }
      } else
      {
         VectorToCenter[0] = OUTER_RADIUS - CurrentPosition[0];
         VectorToCenter[1] = OUTER_RADIUS - CurrentPosition[1];
         // won't be horizontal or vertical, so this works.
         UnitWallVector[0] = cosf((float)RHR_CORRECT*WallAngle DEG_TO_RAD);
         UnitWallVector[1] = sinf((float)RHR_CORRECT*WallAngle DEG_TO_RAD);
         
         DistanceToWallSquared = (VectorToCenter[0]*UnitWallVector[1] - VectorToCenter[1]*UnitWallVector[0])*(VectorToCenter[0]*UnitWallVector[1] - VectorToCenter[1]*UnitWallVector[0]);
         if (DistanceToWallSquared < (SAFE_DISTANCE_FROM_WALL*SAFE_DISTANCE_FROM_WALL))
         {
            return False;
         } else
         {
            return True;
         }            
      }
      //return True; //should never get here
   }     
}



boolean SafeToTravel(float Distance, float CurrentX, float CurrentY, float CurrentTHETA, int WallAngle)
{
   float CurrentPosition[3];
   float NewPosition[2];
   float RadialDistanceSquared;
   float DistanceToWallSquared;
   float VectorToCenter[2];
   float UnitWallVector[2];

   
   CurrentPosition[0] = CurrentX;
   CurrentPosition[1] = CurrentY; 
   CurrentPosition[2] = CurrentTHETA;  
   
   NewPosition[0] = CurrentPosition[0] + Distance*cosf(CurrentPosition[2] DEG_TO_RAD);
   NewPosition[1] = CurrentPosition[1] + Distance*sinf(CurrentPosition[2] DEG_TO_RAD);
   
   RadialDistanceSquared = (CurrentPosition[0] - OUTER_RADIUS)*(CurrentPosition[0] - OUTER_RADIUS)
                            +(CurrentPosition[1] - OUTER_RADIUS)*(CurrentPosition[1] - OUTER_RADIUS);
   if (RadialDistanceSquared > ( RADIUS_OF_SAFETY*RADIUS_OF_SAFETY ) )
   {
      return False;
   } else
   {
      if (WallAngle == 0 || WallAngle == 180)
      {
         if ((NewPosition[1] < (OUTER_RADIUS + SAFE_DISTANCE_FROM_WALL))
               &&(NewPosition[1] > (OUTER_RADIUS - SAFE_DISTANCE_FROM_WALL)))
         {
            return False;
         } else
         {
            return True;
         }
      } else if (WallAngle == 90 || WallAngle == 270)
      {
         if ((NewPosition[0] < (OUTER_RADIUS + SAFE_DISTANCE_FROM_WALL))
               &&(NewPosition[0] > (OUTER_RADIUS - SAFE_DISTANCE_FROM_WALL)))
         {
            return False;
         } else
         {
            return True;
         }
      } else
      {
         VectorToCenter[0] = OUTER_RADIUS - NewPosition[0];
         VectorToCenter[1] = OUTER_RADIUS - NewPosition[1];
         // won't be horizontal or vertical, so this works.
         UnitWallVector[0] = cosf((float)RHR_CORRECT*WallAngle DEG_TO_RAD);
         UnitWallVector[1] = sinf((float)RHR_CORRECT*WallAngle DEG_TO_RAD);
         
         DistanceToWallSquared = (VectorToCenter[0]*UnitWallVector[1] - VectorToCenter[1]*UnitWallVector[0])*(VectorToCenter[0]*UnitWallVector[1] - VectorToCenter[1]*UnitWallVector[0]);
         if (DistanceToWallSquared < (SAFE_DISTANCE_FROM_WALL*SAFE_DISTANCE_FROM_WALL))
         {
            return False;
         } else
         {
            return True;
         }            
      }
      //return True; //should never get here
   }        
}


float GetSeparationDistance(float GoalX,float GoalY,float CurrentX,float CurrentY)
{
   float dSquared;
   float d;
   
   dSquared = (GoalX-CurrentX)*(GoalX-CurrentX) + (GoalY-CurrentY)*(GoalY-CurrentY);
   d = sqrt(dSquared);
   return d;   
}


float GetSeparationAngle(float CurrentPosition[3],float GoalPosition[2])
{
   float CurrentTHETA = CurrentPosition[2];
   float GoalX = GoalPosition[0];
   float GoalY = GoalPosition[1];
   float CurrentX = CurrentPosition[0];
   float CurrentY = CurrentPosition[1];
   
   float AngleFromCurrentToGoal;
   
   float phi;
   
   AngleFromCurrentToGoal = atan2((GoalY-CurrentY),(GoalX-CurrentX)) RAD_TO_DEG;
   
   //printf("raw PHI is %f\n\r", VectorFromCurrentToGoalAngle);
   
   // this block of code checks to see whether it's more efficient to turn right or left.
   phi = AngleFromCurrentToGoal - (CurrentTHETA);

   if (phi >= 180)
   {
      phi = phi - 360.;
   } else if (phi <= -180)
   {
      phi = 360. + phi;
   }

   return phi;  //in radians 
}

static ES_Event GenerateNextStep_GoalPoint(float CurrentPosition[3], float GoalOne[2], int EventParameter)
{
   ES_Event ReturnEvent;
   float d;
   float phi;
   d = GetSeparationDistance(GoalOne[0],GoalOne[1],CurrentPosition[0],CurrentPosition[1]);
   printf("distance to goal is %f \n\r", d);
   phi = GetSeparationAngle(CurrentPosition,GoalOne);
   printf("phi to goal is %f degrees \n\r", phi);
   
   if (fabs(phi)>ANGLE_ERROR)
   {
      ReturnEvent.EventType = TURN_COMMAND;
      ReturnEvent.EventParam = (int)phi;  
   } else if((d>=POSITION_ERROR) && (d<=(STEP_DISTANCE/100. + POSITION_ERROR*1.1)))  /*1.1*/
   {
      ReturnEvent.EventType = GO_COMMAND;
      ReturnEvent.EventParam = (int)(d*100);   
   } else if (d > (STEP_DISTANCE/100. + POSITION_ERROR*1.1))  /*1.1*/
   {
      ReturnEvent.EventType = GO_COMMAND;
      ReturnEvent.EventParam = STEP_DISTANCE;
   } else
   {
      /*
      float NewPhi;
      NewPhi = GetSeparationAngle(CurrentPosition,GoalTwo); 
      ReturnEvent.EventType = TURN_COMMAND;
      ReturnEvent.EventParam =(int)NewPhi;
      */
       ReturnEvent.EventType = WAIT_COMMAND;
       ReturnEvent.EventParam = EventParameter;
       // update check points!
       //BinCheckPointTwo = True;
       BinCheckPoints++;
       WallCheckPoints++;
    }
   printf("Goal Point Path\n\r");

   return ReturnEvent;
   
}

static ES_Event GenerateNextStep_RadialPath(float CurrentPosition[3],float FinalDistanceFromCenter, int EventParameter)
{
   float GoalPoint[2];
   float d;
   float phi;
   ES_Event ReturnEvent;
   if ( CurrentPosition[0]==OUTER_RADIUS)  // deal with vertical case  
   {
      GoalPoint[0]=OUTER_RADIUS;
      GoalPoint[1] = OUTER_RADIUS+FinalDistanceFromCenter;
   } else 
   {
      float GoalTheta = (atan2((CurrentPosition[1]-OUTER_RADIUS),(CurrentPosition[0]-OUTER_RADIUS)));
      GoalPoint[0]=OUTER_RADIUS + FinalDistanceFromCenter*cosf(GoalTheta);//??????
      GoalPoint[1]=OUTER_RADIUS + FinalDistanceFromCenter*sinf(GoalTheta);   
   }
   
   
   d = GetSeparationDistance(GoalPoint[0],GoalPoint[1],CurrentPosition[0],CurrentPosition[1]);
   phi = GetSeparationAngle(CurrentPosition,GoalPoint);
   
   if (fabs(phi)>ANGLE_ERROR)
   {
      ReturnEvent.EventType = TURN_COMMAND;
      ReturnEvent.EventParam = (int)phi;        
   } else if(d>POSITION_ERROR && d<(STEP_DISTANCE/100. + 1.1*POSITION_ERROR))
   {
      ReturnEvent.EventType = GO_COMMAND;
      ReturnEvent.EventParam = (int)(d*100);   
   } else if (d > (STEP_DISTANCE/100. + 1.1*POSITION_ERROR))
   {
      ReturnEvent.EventType = GO_COMMAND;
      ReturnEvent.EventParam = STEP_DISTANCE;
   } else
   {
       //ES_Event ActionEvent;
       //turn off hoppers.
       
       ReturnEvent.EventType = WAIT_COMMAND;
       ReturnEvent.EventParam = EventParameter;
       // update check points!
       WallCheckPoints++;
       //BinCheckPointOne = True;
       BinCheckPoints++;
       //ReturnEvent.EventType = TURN_COMMAND;    // Necessary?
       //ReturnEvent.EventParam = (int)phi;
   }
   
   printf("Radial Path\n\r");// Goal Point x:%f y:%f d:%f phi:%f \n\r",GoalPoint[0],GoalPoint[1],d,phi);
   
   return (ReturnEvent);                         
}

static ES_Event GenerateNextStep_EscapeWallPath(float CurrentPosition[3],float WallAngle)
{
   float GoalPointOne[2];
   float GoalPointTwo[2];
   float FinalGoalPoint[2];
   float xOffset;
   float yOffset;
   float carDistanceSquaredToGoal_ONE;
   float carDistanceSquaredToGoal_TWO;
   float d;
   float phi;
   ES_Event ReturnEvent;  
   

   if (WallAngle == 0 || WallAngle == 180)
      {                                    
         GoalPointOne[0] = OUTER_RADIUS;     //x1
         GoalPointOne[1] = OUTER_RADIUS/2;   //y1
         GoalPointTwo[0] = OUTER_RADIUS;     //x2
         GoalPointTwo[1] = 3*OUTER_RADIUS/2; //y2
         
      } else if (WallAngle == 90 || WallAngle == 270)
      {         
         GoalPointOne[0] = OUTER_RADIUS/2;   //x1
         GoalPointOne[1] = OUTER_RADIUS;     //y1
         GoalPointTwo[0] = 3*OUTER_RADIUS/2; //x2
         GoalPointTwo[1] = OUTER_RADIUS;     //y2
         
      } else
      {
         xOffset = OUTER_RADIUS*cosf((90. DEG_TO_RAD) + (float)(RHR_CORRECT*WallAngle DEG_TO_RAD))/2;
         yOffset = OUTER_RADIUS*sinf((90. DEG_TO_RAD) + (float)(RHR_CORRECT*WallAngle DEG_TO_RAD))/2; 
         
         GoalPointOne[0] = OUTER_RADIUS + xOffset;     //x1
         GoalPointOne[1] = OUTER_RADIUS + yOffset;     //y1
         GoalPointTwo[0] = OUTER_RADIUS - xOffset;     //x2
         GoalPointTwo[1] = OUTER_RADIUS - yOffset;     //y2
           
      } 
   
   carDistanceSquaredToGoal_ONE = ( GoalPointOne[0] - CurrentPosition[0] )*( GoalPointOne[0] - CurrentPosition[0] )
                                    + ( GoalPointOne[1] - CurrentPosition[1] )*( GoalPointOne[1] - CurrentPosition[1] );
   carDistanceSquaredToGoal_TWO = ( GoalPointTwo[0] - CurrentPosition[0] )*( GoalPointTwo[0] - CurrentPosition[0] ) 
                                    + ( GoalPointTwo[1] - CurrentPosition[1] )*( GoalPointTwo[1] - CurrentPosition[1] );   
   if (carDistanceSquaredToGoal_ONE < carDistanceSquaredToGoal_TWO)
   { 
      FinalGoalPoint[0] = GoalPointOne[0];
      FinalGoalPoint[1] = GoalPointOne[1];
   } else 
   {
      FinalGoalPoint[0] = GoalPointTwo[0];
      FinalGoalPoint[1] = GoalPointTwo[1];
   }
   
   d = GetSeparationDistance(FinalGoalPoint[0],FinalGoalPoint[1],CurrentPosition[0],CurrentPosition[1])*100;
   phi = GetSeparationAngle(CurrentPosition,FinalGoalPoint);
   
   if (fabs(phi)>ANGLE_ERROR)
   {
      ReturnEvent.EventType = TURN_COMMAND;
      ReturnEvent.EventParam = (int)phi;        
   } else if(d>POSITION_ERROR && d<(STEP_DISTANCE/100. + 1.1*POSITION_ERROR))
   {
      ReturnEvent.EventType = GO_COMMAND;
      ReturnEvent.EventParam = (int)(d*100);   
   } else if (d > (STEP_DISTANCE/100. + 1.1*POSITION_ERROR))
   {
      ReturnEvent.EventType = GO_COMMAND;
      ReturnEvent.EventParam = STEP_DISTANCE;
   } else
   {
       ReturnEvent.EventType = WAIT_COMMAND;
       ReturnEvent.EventParam = WAIT_FOR_POSITION_FOUND;
       //ReturnEvent.EventType = TURN_COMMAND;    // Necessary?
       //ReturnEvent.EventParam = (int)phi;
   }
   
   printf("Escape Wall Path\n\r");// Goal Point x1:%f y1:%f x2:%f y2:%f xOffset:%f yOffset:%f \n\r",GoalPointOne[0],GoalPointOne[1],GoalPointTwo[0],GoalPointTwo[1], xOffset, yOffset);
   //printf("Separation Angle is %f\n\r",phi);   
   return (ReturnEvent);
}  


void CheckPointsPlusPlus(void)
{
   BinCheckPoints++;
   WallCheckPoints++;
}