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
#include "IRSM.h"
#include "StepperSM.h"
#include "ES_PostList.h"
#include "math.h"
#include <stdio.h>	
#include <stdlib.h>
#include "GameConstants.h"

/*----------------------------- Module Defines ----------------------------*/



/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/

void honingStartDirection(int );
void beaconVar(int , int);
void Triangulate(float, float, float, float);

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static TemplateState_t CurrentState;

static int BeaconArray[4] = {3750, 3375, 3000, 2625};
//static int ScanStepCountArray[4] = { 0 };
static int ScanStepPositionArray[4] = { 0 };
static int StepPositionAverageArray[4] = { 0 };
static int beaconCount = 0;
static int LastBinCode;
static int TemporaryStepPositionArray[4] = { 0 };
static unsigned int uLastPulse = 0;
static unsigned int uPeriod = 0;

static float thetaR;
static float xPosition;
static float yPosition;

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
boolean InitIRSM ( uint8_t Priority )
{
  ES_Event ThisEvent;

  MyPriority = Priority; 
  // put us into the Initial PseudoState
  CurrentState = InitPState_IR;

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
boolean PostIRSM( ES_Event ThisEvent )
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
ES_Event RunIRSM( ES_Event ThisEvent )
{
	ES_Event ReturnEvent;
	ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
	
	switch ( CurrentState )
	{
		case InitPState_IR :
			if ( ThisEvent.EventType == ES_INIT )
			{
			  CurrentState = Holding;
			  //printf("IRSM---->Move to state: Holding\n\r");
			}
			break;

		case Holding :
		  switch ( ThisEvent.EventType )
		  {
               case HONE_BIN1 : 
                  CurrentState = Honing_Bin1;
				  //printf("IRSM---->Move to state: Honing_Bin1\n\r");
				  honingStartDirection(BIN_1_CODE);
                  break;
				  
			   case HONE_BIN2: 
                  CurrentState = Honing_Bin2;
				  //printf("IRSM---->Move to state: Honing_Bin2\n\r");
				  honingStartDirection(BIN_2_CODE);
                  break;
				  
			   case HONE_BIN3: 
                  CurrentState = Honing_Bin3;
				  //printf("IRSM---->Move to state: Honing_Bin3\n\r");
				  honingStartDirection(BIN_3_CODE);
                  break;

			   case HONE_BIN4: 
                  CurrentState = Honing_Bin4;
				  //printf("IRSM---->Move to state: Honing_Bin4\n\r");
				  honingStartDirection(BIN_4_CODE);
                  break;
				  
				case FIND_POSITION:
				{
				  ES_Event StepperEvent;
				  StepperEvent.EventType = STEP_CW;
				  StepperEvent.EventParam = 1;
				  ES_PostList07(StepperEvent);
				  //resetStepper();
				  CurrentState = Zeroing;
				  printf("IRSM---->Move to state: Zeroing\n\r");
				}
					break;
					
				/*case ZERO_AND_STAY:	
				{
				  ES_Event StepperEvent;
				  StepperEvent.EventType = STEP_CW;
				  StepperEvent.EventParam = 1;
				  ES_PostList07(StepperEvent);
				  //resetStepper();
				  CurrentState = Zeroing;
				  printf("IRSM---->Move to state: Zeroing\n\r");
				}
					break;*/
		  }
		  break;
		  
		case Honing_Bin1:
		  switch( ThisEvent.EventType )
		  {
               case BEACON_BIN1_RIGHT_LOST:
               { 
				  ES_Event StepperEvent;
				  StepperEvent.EventType = STEP_CCW;
				  StepperEvent.EventParam = 1;
				  ES_PostList07(StepperEvent); 
               }
                  break;

               case BEACON_BIN1_LEFT_LOST: 
               {
				  ES_Event StepperEvent;
				  StepperEvent.EventType = STEP_CW;
				  StepperEvent.EventParam = 1;
				  ES_PostList07(StepperEvent); 
               }
                  break;
                
               case BEACON_BIN1_HONED: 
               {
				  ES_Event StepperEvent;
				  StepperEvent.EventType = STOP_STEPPING;
				  StepperEvent.EventParam = 1;
				  ES_PostList07(StepperEvent); 
               }
                  break;                             
				  
			   case HONE_BIN2: 
                  CurrentState = Honing_Bin2;
				  //printf("IRSM---->Move to state: Honing_Bin2\n\r");
				  honingStartDirection(BIN_2_CODE);
                  break;
				  
			   case HONE_BIN3: 
                  CurrentState = Honing_Bin3;
				  //printf("IRSM---->Move to state: Honing_Bin3\n\r");
				  honingStartDirection(BIN_3_CODE);
                  break;

			   case HONE_BIN4: 
                  CurrentState = Honing_Bin4;
				  //printf("IRSM---->Move to state: Honing_Bin4\n\r");
				  honingStartDirection(BIN_4_CODE);
                  break;	
				  
			   case FIND_POSITION:
			   {
				  ES_Event StepperEvent;
				  StepperEvent.EventType = STEP_CW;
				  StepperEvent.EventParam = 1;
				  ES_PostList07(StepperEvent);
				  //resetStepper();
				  CurrentState = Zeroing;
                }
				  break;	
				  
			   case STOP_STEPPING:
			   {
				  CurrentState = Holding;
			   }  
                  break;					  
		  }
		  break;
		  
		case Honing_Bin2:
		  switch( ThisEvent.EventType )
		  {
               case BEACON_BIN2_RIGHT_LOST:
               { 
				  ES_Event StepperEvent;
				  StepperEvent.EventType = STEP_CW;
				  StepperEvent.EventParam = 1;
				  ES_PostList07(StepperEvent); 
               }
                  break;

               case BEACON_BIN2_LEFT_LOST: 
               {
				  ES_Event StepperEvent;
				  StepperEvent.EventType = STEP_CCW;
				  StepperEvent.EventParam = 1;
				  ES_PostList07(StepperEvent); 
               }
                  break;
                
               case BEACON_BIN2_HONED: 
               {
				  ES_Event StepperEvent;
				  StepperEvent.EventType = STOP_STEPPING;
				  StepperEvent.EventParam = 1;
				  ES_PostList07(StepperEvent); 
               }
                  break;                             
				  
			   case HONE_BIN1: 
                  CurrentState = Honing_Bin1;
				  //printf("IRSM---->Move to state: Honing_Bin1\n\r");
				  honingStartDirection(BIN_1_CODE);
                  break;
				  
			   case HONE_BIN3: 
                  CurrentState = Honing_Bin3;
				  //printf("IRSM---->Move to state: Honing_Bin3\n\r");
				  honingStartDirection(BIN_3_CODE);
                  break;

			   case HONE_BIN4: 
                  CurrentState = Honing_Bin4;
				  //printf("IRSM---->Move to state: Honing_Bin4\n\r");
				  honingStartDirection(BIN_4_CODE);
                  break;	
				  
			   case FIND_POSITION:
			   {
				  ES_Event StepperEvent;
				  StepperEvent.EventType = STEP_CW;
				  StepperEvent.EventParam = 1;
				  ES_PostList07(StepperEvent);
				  //resetStepper();
				  CurrentState = Zeroing;
                }
				  break;
				  
			   case STOP_STEPPING:
			   {
				  CurrentState = Holding;
			   }  
                  break;	
		  }
		  break;		 

		case Honing_Bin3:
		  switch( ThisEvent.EventType )
		  {
               case BEACON_BIN3_RIGHT_LOST:
               { 
				  ES_Event StepperEvent;
				  StepperEvent.EventType = STEP_CW;
				  StepperEvent.EventParam = 1;
				  ES_PostList07(StepperEvent); 
               }
                  break;

               case BEACON_BIN3_LEFT_LOST: 
               {
				  ES_Event StepperEvent;
				  StepperEvent.EventType = STEP_CCW;
				  StepperEvent.EventParam = 1;
				  ES_PostList07(StepperEvent); 
               }
                  break;
                
               case BEACON_BIN3_HONED: 
               {
				  ES_Event StepperEvent;
				  StepperEvent.EventType = STOP_STEPPING;
				  StepperEvent.EventParam = 1;
				  ES_PostList07(StepperEvent); 
               }
                  break;                             
				  
			   case HONE_BIN1: 
                  CurrentState = Honing_Bin1;
				  //printf("IRSM---->Move to state: Honing_Bin1\n\r");
				  honingStartDirection(BIN_1_CODE);
                  break;
				  
			   case HONE_BIN2: 
                  CurrentState = Honing_Bin2;
				  //printf("IRSM---->Move to state: Honing_Bin2\n\r");
				  honingStartDirection(BIN_2_CODE);
                  break;

			   case HONE_BIN4: 
                  CurrentState = Honing_Bin4;
				  //printf("IRSM---->Move to state: Honing_Bin4\n\r");
				  honingStartDirection(BIN_4_CODE);
                  break;	
				  
			   case FIND_POSITION:
			   {
				  ES_Event StepperEvent;
				  StepperEvent.EventType = STEP_CW;
				  StepperEvent.EventParam = 1;
				  ES_PostList07(StepperEvent);
				  //resetStepper();
				  CurrentState = Zeroing;
                }
				  break;
				  
			   case STOP_STEPPING:
			   {
				  CurrentState = Holding;
			   }  
                  break;					  
		  }
		  break;	
		  
		case Honing_Bin4:
		  switch( ThisEvent.EventType )
		  {
               case BEACON_BIN4_RIGHT_LOST:
               { 
				  ES_Event StepperEvent;
				  StepperEvent.EventType = STEP_CW;
				  StepperEvent.EventParam = 1;
				  ES_PostList07(StepperEvent); 
               }
                  break;

               case BEACON_BIN4_LEFT_LOST: 
               {
				  ES_Event StepperEvent;
				  StepperEvent.EventType = STEP_CCW;
				  StepperEvent.EventParam = 1;
				  ES_PostList07(StepperEvent); 
               }
                  break;
                
               case BEACON_BIN4_HONED: 
               {
				  ES_Event StepperEvent;
				  StepperEvent.EventType = STOP_STEPPING;
				  StepperEvent.EventParam = 1;
				  ES_PostList07(StepperEvent); 
               }
                  break;                             
				  
			   case HONE_BIN1: 
                  CurrentState = Honing_Bin1;
				  //printf("IRSM---->Move to state: Honing_Bin1\n\r");
				  honingStartDirection(BIN_1_CODE);
                  break;
				  
			   case HONE_BIN2: 
                  CurrentState = Honing_Bin2;
				  //printf("IRSM---->Move to state: Honing_Bin2\n\r");
				  honingStartDirection(BIN_2_CODE);
                  break;

			   case HONE_BIN3: 
                  CurrentState = Honing_Bin3;
				  //printf("IRSM---->Move to state: Honing_Bin3\n\r");
				  honingStartDirection(BIN_3_CODE);
                  break;	
				  
			   case FIND_POSITION:
			   {
				  ES_Event StepperEvent;
				  StepperEvent.EventType = STEP_CW;
				  StepperEvent.EventParam = 1;
				  ES_PostList07(StepperEvent);
				  //resetStepper();
				  CurrentState = Zeroing;
                }
				  break;	

			   case STOP_STEPPING:
			   {
				  CurrentState = Holding;
			   }  
                  break;	  
		  }
		  break;			  

		case Zeroing:
		  switch( ThisEvent.EventType )
		  {
			case ZERO_FOUND:
			   {
				  ES_Event StepperEvent;
				  StepperEvent.EventType = STEP_CCW;
				  StepperEvent.EventParam = 1;
				  ES_PostList07(StepperEvent);
				  //resetStepper();
				  ScanStepPositionArray[0] = 0;
	           ScanStepPositionArray[1] = 0;
		        ScanStepPositionArray[2] = 0;
		        ScanStepPositionArray[3] = 0;
		        StepPositionAverageArray[0] = 0;
		        StepPositionAverageArray[1] = 0;
		        StepPositionAverageArray[2] = 0;
		        StepPositionAverageArray[3] = 0;
		        LastBinCode = BAD_BIN;		
		        beaconCount = 0;	
				  CurrentState = Positioning;
				  printf("IRSM---->Move to state: Positioning\n\r");
               }
				  break;
				  
               case STOP_STEPPING:
			   {
				  CurrentState = Holding;
			   }  
                  break;					  
		  }
		  break;	
		  
		case Positioning:
		  switch( ThisEvent.EventType )
		  {
               case BEACON_BIN1_HONED:
				  beaconVar(BIN_1_CODE, ThisEvent.EventParam);
                  break;       

               case BEACON_BIN2_HONED:
				  beaconVar(BIN_2_CODE, ThisEvent.EventParam);
                  break;   

               case BEACON_BIN3_HONED:
				  beaconVar(BIN_3_CODE, ThisEvent.EventParam);
                  break;   

               case BEACON_BIN4_HONED:
				  beaconVar(BIN_4_CODE, ThisEvent.EventParam);
                  break;  

               case THREE_FOUND:
			   {
				  ES_Event PositionEvent;
				  PositionEvent.EventType = POSITION_FOUND;
			      PositionEvent.EventParam = 1;
				  ES_PostList02(PositionEvent);				  
				  CurrentState = Holding;
			   }  
                  break;

			   case ZERO_FOUND:
			   {
				  ES_Event StepperEvent;
				  StepperEvent.EventType = STEP_CW;
				  StepperEvent.EventParam = 1;
				  ES_PostList07(StepperEvent);
				  //resetStepper();
				  CurrentState = Zeroing;
				  printf("IRSM---->Move to state: Zeroing\n\r");
               }
				  break;				  
				  
               case STOP_STEPPING:
			   {
				  CurrentState = Holding;
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
/*TemplateState_t QueryIRSM ( void )
{
   return(CurrentState);
}*/ 

/***************************************************************************
 access functions
 ***************************************************************************/
 
float accessXPosition(void)
{
   return xPosition;
}

float accessYPosition(void)
{
   return yPosition;
}

float accessAngle(void)
{
   return thetaR;
}    
 
 /***************************************************************************
 private functions
 ***************************************************************************/


// UNTESTED 3/5/12
void honingStartDirection(int binCode){
  ES_Event StepperEvent;
  
  if ( accessStepCount() > TemporaryStepPositionArray[binCode]){
	  StepperEvent.EventType = STEP_CW;
	  StepperEvent.EventParam = 1;
	  ES_PostList07(StepperEvent); 
	  }
  else if ( accessStepCount() <= TemporaryStepPositionArray[binCode]){	  
	  StepperEvent.EventType = STEP_CCW;
	  StepperEvent.EventParam = 1;
	  ES_PostList07(StepperEvent);  
  }
}

void beaconVar(int binCode, int currentStep){
	
	if (LastBinCode == binCode)
	{
	   StepPositionAverageArray[binCode] = (ScanStepPositionArray[binCode]+currentStep)/2;
	}
	
	
	if (LastBinCode != binCode)
		{	
		beaconCount++;
		ScanStepPositionArray[binCode] = currentStep;	
		StepPositionAverageArray[binCode] = currentStep;
		
		TemporaryStepPositionArray[binCode] = currentStep;
		printf("Beacon count updated: %d\n\r", beaconCount);
		//printf("Bin code: %d\n\r", binCode);
		}	
	LastBinCode = binCode;
	
	
	if (beaconCount == 3) {
		ES_Event StepperEvent;
		ES_Event IREvent;
		
		StepperEvent.EventType = STOP_STEPPING;
		StepperEvent.EventParam = 1;
		ES_PostList07(StepperEvent);	
		
		
		//printf("Last Position x,y,theta: %f, %f, %f\n\r",xPosition, yPosition, thetaR);
		/*
		printf("Triangulating -- Angle1: %f, Angle2: %f, Angle3: %f, Angle4: %f\n\r", ((float)ScanStepPositionArray[0])*HALF_STEP_MULTIPLIER,
																					  ((float)ScanStepPositionArray[1])*HALF_STEP_MULTIPLIER,
																					  ((float)ScanStepPositionArray[2])*HALF_STEP_MULTIPLIER,
																					  ((float)ScanStepPositionArray[3])*HALF_STEP_MULTIPLIER);
		Triangulate(((float)ScanStepPositionArray[0])*HALF_STEP_MULTIPLIER,
					((float)ScanStepPositionArray[1])*HALF_STEP_MULTIPLIER,
					((float)ScanStepPositionArray[2])*HALF_STEP_MULTIPLIER,
					((float)ScanStepPositionArray[3])*HALF_STEP_MULTIPLIER);
					   
		ScanStepPositionArray[0] = 0;
		ScanStepPositionArray[1] = 0;
		ScanStepPositionArray[2] = 0;
		ScanStepPositionArray[3] = 0;*/
		
		/*
		printf("Triangulating -- Angle1: %f, Angle2: %f, Angle3: %f, Angle4: %f\n\r", ((float)ScanStepPositionArray[0])*MICRO_STEP_MULTIPLIER,
																					  ((float)ScanStepPositionArray[1])*MICRO_STEP_MULTIPLIER,
																					  ((float)ScanStepPositionArray[2])*MICRO_STEP_MULTIPLIER,
																					  ((float)ScanStepPositionArray[3])*MICRO_STEP_MULTIPLIER);
		Triangulate(((float)ScanStepPositionArray[0])*MICRO_STEP_MULTIPLIER,
					((float)ScanStepPositionArray[1])*MICRO_STEP_MULTIPLIER,
					((float)ScanStepPositionArray[2])*MICRO_STEP_MULTIPLIER,
					((float)ScanStepPositionArray[3])*MICRO_STEP_MULTIPLIER); */
					
	//	printf("Triangulating -- Angle1: %f, Angle2: %f, Angle3: %f, Angle4: %f\n\r", ((float)ScanStepPositionArray[0])*MICRO_STEP_MULTIPLIER,
																			  //((float)StepPositionAverageArray[1])*MICRO_STEP_MULTIPLIER,
																			  //((float)StepPositionAverageArray[2])*MICRO_STEP_MULTIPLIER,
																			  //((float)StepPositionAverageArray[3])*MICRO_STEP_MULTIPLIER);
		Triangulate(((float)StepPositionAverageArray[0])*MICRO_STEP_MULTIPLIER,
					((float)StepPositionAverageArray[1])*MICRO_STEP_MULTIPLIER,
					((float)StepPositionAverageArray[2])*MICRO_STEP_MULTIPLIER,
					((float)StepPositionAverageArray[3])*MICRO_STEP_MULTIPLIER);
	   
	   
	   IREvent.EventType = THREE_FOUND;
		IREvent.EventParam = 1;
		ES_PostList05(IREvent);
		

	}

}

void Triangulate(float angle1, float angle2, float angle3, float angle4)
{
	float phi;
	float sigma;
	float gamma;
	float tau;
	float len;
	float lambda12;
	float lambda31;
	float len12 = LENGTH;
	float len31 = LENGTH;
	float beaconX = LENGTH;
	float beaconY = LENGTH;
	
	if ( angle4 == 0 ){
		phi = 0 DEG_TO_RAD;
		sigma = 135 DEG_TO_RAD;
		angle1 = angle1 DEG_TO_RAD;
		angle2 = angle2 DEG_TO_RAD;
		angle3 = angle3 DEG_TO_RAD;
		len31 = sqrt(2) * len31;
	} else if ( angle3 == 0 ){
		phi = 0 DEG_TO_RAD;
		sigma = 90 DEG_TO_RAD;
		angle1 = angle1 DEG_TO_RAD;
		angle2 = angle2 DEG_TO_RAD;
		angle3 = angle4 DEG_TO_RAD;
	} else if ( angle2 == 0 ){
		phi = 45 DEG_TO_RAD;
		sigma = 135 DEG_TO_RAD;
		angle1 = angle1 DEG_TO_RAD;
		angle2 = angle3 DEG_TO_RAD;
		angle3 = angle4 DEG_TO_RAD;
		len12 = sqrt(2) * len12;
	} else if ( angle1 == 0 ){
		phi = 90 DEG_TO_RAD;
		sigma = 135 DEG_TO_RAD;
		angle1 = angle2 DEG_TO_RAD;
		angle2 = angle3 DEG_TO_RAD;
		angle3 = angle4 DEG_TO_RAD;
		len31 = sqrt(2) * len31;
		beaconX = 0;
	};
	
	lambda12 = angle2-angle1;
	lambda31 = angle1-angle3;
	
	if (angle1 > angle2){
		lambda12 = 360 DEG_TO_RAD + lambda12;
	}
	
	if (angle3 > angle1){
		lambda31 = 360 DEG_TO_RAD + lambda31;
	}
	
	gamma = sigma - lambda31;

	tau = atanf( ( sinf(lambda12) * (len12*sinf(lambda31) - len31*sinf(gamma)) ) /
				 ( len31*sinf(lambda12)*cosf(gamma) - len12*cosf(lambda12)*sinf(lambda31) ) );	 
				 
	
	if ( (lambda12 < (180 DEG_TO_RAD)) && (tau < 0) ){
		tau = tau + (180 DEG_TO_RAD);
	}
	
	if ( (lambda12 > (180 DEG_TO_RAD)) && (tau > 0) ){
		tau = tau - (180 DEG_TO_RAD);
	}
	
	if ( fabs(sinf(lambda12)) > fabs(sinf(lambda31)) ){
		len = len12 * sinf(tau + lambda12) / sinf(lambda12);
	} else{
		len = len31 * sinf(tau + sigma - lambda31) / sinf(lambda31);
	}
	
	xPosition = beaconX - len * cosf(phi + tau) + 1 /*convert to board coordinate*/;
	yPosition = beaconY - len * sinf(phi + tau) + 1 /*convert to board coordinate*/;
	thetaR = phi + tau - angle1;
	
	if ( thetaR <= -(180 DEG_TO_RAD) ){
		thetaR = thetaR + (360 DEG_TO_RAD);
	}
	
	if ( thetaR > (180 DEG_TO_RAD) ){
		thetaR = thetaR - (360 DEG_TO_RAD);
	}
	
	//thetaR = thetaR RAD_TO_DEG;
	
	// correction for beacon error:
	thetaR = (thetaR RAD_TO_DEG);// - 6;
	
//	printf("xPosition: %f  yPosition: %f  theta: %f\n\r", xPosition, yPosition, thetaR);
	
}