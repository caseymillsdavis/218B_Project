/****************************************************************************
 Module
     ES_Configure.h
 Description
     This file contains macro definitions that are edited by the user to
     adapt the Events and Services framework to a particular application.
 Notes
     
 History
 When           Who     What/Why
 -------------- ---     --------
 01/15/12 10:03 jec      started coding
*****************************************************************************/

#ifndef CONFIGURE_H
#define CONFIGURE_H

/****************************************************************************/
// The maximum number of services sets an upper bound on the number of 
// services that the framework will handle. Reasonable values are 8 and 16
// HOWEVER: at this time only a value of 8 is supported.
#define MAX_NUM_SERVICES 8

/****************************************************************************/
// This macro determines that nuber of services that are *actually* used in
// a particular application. It will vary in value from 1 to MAX_NUM_SERVICES
#define NUM_SERVICES 6

/****************************************************************************/
// These are the definitions for Service 0, the lowest priority service
// every Events and Services application must have a Service 0. Further 
// services are added in numeric sequence (1,2,3,...) with increasing 
// priorities
// the header file with the public fuction prototypes
#define SERV_0_HEADER "TankSM.h"
// the name of the Init function
#define SERV_0_INIT InitTankSM
// the name of the run function
#define SERV_0_RUN RunTankSM
// How big should this services Queue be?
#define SERV_0_QUEUE_SIZE 16

/****************************************************************************/
// The following sections are used to define the parameters for each of the
// services. You only need to fill out as many as the number of services 
// defined by NUM_SERVICES
/****************************************************************************/
// These are the definitions for Service 1
#if NUM_SERVICES > 1
// the header file with the public fuction prototypes
#define SERV_1_HEADER "ThinkTankSM.h"
// the name of the Init function
#define SERV_1_INIT InitThinkTankSM
// the name of the run function
#define SERV_1_RUN RunThinkTankSM
// How big should this services Queue be?
#define SERV_1_QUEUE_SIZE 3
#endif

/****************************************************************************/
// These are the definitions for Service 2
#if NUM_SERVICES > 2
// the header file with the public fuction prototypes
#define SERV_2_HEADER "QuerySM.h"
// the name of the Init function
#define SERV_2_INIT InitQuerySM
// the name of the run function
#define SERV_2_RUN RunQuerySM
// How big should this services Queue be?
#define SERV_2_QUEUE_SIZE 3
#endif

/****************************************************************************/
// These are the definitions for Service 3
#if NUM_SERVICES > 3
// the header file with the public fuction prototypes
#define SERV_3_HEADER "GameMonitorService.h"
// the name of the Init function
#define SERV_3_INIT InitGameMonitorService
// the name of the run function
#define SERV_3_RUN RunGameMonitorService
// How big should this services Queue be?
#define SERV_3_QUEUE_SIZE 3
#endif

/****************************************************************************/
// These are the definitions for Service 4
#if NUM_SERVICES > 4
// the header file with the public fuction prototypes
#define SERV_4_HEADER "IRSM.h"
// the name of the Init function
#define SERV_4_INIT InitIRSM
// the name of the run function
#define SERV_4_RUN RunIRSM
// How big should this services Queue be?
#define SERV_4_QUEUE_SIZE 3
#endif

/****************************************************************************/
// These are the definitions for Service 5
#if NUM_SERVICES > 5
// the header file with the public fuction prototypes
#define SERV_5_HEADER "StepperSM.h"
// the name of the Init function
#define SERV_5_INIT InitStepperSM
// the name of the run function
#define SERV_5_RUN RunStepperSM
// How big should this services Queue be?
#define SERV_5_QUEUE_SIZE 3
#endif

/****************************************************************************/
// These are the definitions for Service 6
#if NUM_SERVICES > 6
// the header file with the public fuction prototypes
#define SERV_6_HEADER "TestService.h"
// the name of the Init function
#define SERV_6_INIT TestServiceInit
// the name of the run function
#define SERV_6_RUN TestServiceRun
// How big should this services Queue be?
#define SERV_6_QUEUE_SIZE 3
#endif

/****************************************************************************/
// These are the definitions for Service 7
#if NUM_SERVICES > 7
// the header file with the public fuction prototypes
#define SERV_7_HEADER "TestService.h"
// the name of the Init function
#define SERV_7_INIT TestServiceInit
// the name of the run function
#define SERV_7_RUN TestServiceRun
// How big should this services Queue be?
#define SERV_7_QUEUE_SIZE 3
#endif

/****************************************************************************/
// the name of the posting function that you want executed when a new 
// keystroke is detected.
// The default initialization distributes keystrokes to all state machines
#define POST_KEY_FUNC ES_PostAll

/****************************************************************************/
// Name/define the events of interest
// Universal events occupy the lowest entries, followed by user-defined events
typedef enum {  ES_NO_EVENT = 0,
                ES_ERROR,  /* used to indicate an error from the service */
                ES_INIT,   /* used to transition from initial pseudo-state */
                ES_NEW_KEY, /* signals a new key received from terminal */
				    ES_TIMEOUT, /* signals that the timer has expired */
                /* User-defined events start here */
                
                /**Universal Hierarchical State Machine Events*************/
                ES_ENTRY,
                ES_ENTRY_HISTORY,
                ES_EXIT,
                /**********************************************************/
             
                MASTER_RESET,  // CheckForMasterReset posts this to the top level of Tank (and ThinkTank)                
                
                BALLS_IN_PLAY, // BallChecker posts this event to the top level of Tank and ThinkTank
                BIN_CHANGED, //BinChecker posts this to ThinkTank => GameControl
				    WALL_MOVED, //WallAngleChecker posts this to ThinkTank => GameControl
                
                WALL_CHOSEN, //ChooseWallRoutine() posts this to ThinkTank => GameControl
                BIN_CHOSEN, //ChooseBinRoutine() posts this to ThinkTank => GameControl
                DEPOSIT_COMPLETE, //DepositComplete(?) posts this to ThinkTank AND to Tank => HopperDrive
				    FAVORITE_BIN_THREATENED, //CheckForBinThreat(?) posts this to ThinkTank => GameControl
                GOAL_ANGLE_ACHIEVED, //WallAngleOptimal(?) posts this to ThinkTank => GameControl
                EPIC_PUSH_FAIL, //WallAngleOptimal(?) posts this to ThinkTank => GameControl
                DEPOSIT_TIME, //GameControl posts this to itself based on a timeout event.
                
                //HOLD_COMMAND, //PositionControl posts this to Tank=>TreadDrive
                //DRIVE_COMMAND, //PositionControl posts this to Tank=>TreadDrive
                //ROTATE_COMMAND, //PositionControl posts this to Tank=>TreadDrive
                
                ROTATION_COMPLETE, //RotationRoutine(?) posts this to ThinkTank => PositionControl
                ARRIVED, //DriveDistanceRoutine(?) posts this to ThinkTank => PositionControl
                POSITION_FOUND, //FindPositionRoutine(?) posts this to ThinkTank => PositionControl
                
                TURN_COMMAND, //PathPlanner posts this to ThinkTank => PositionControl
                GO_COMMAND, //PathPlanner posts this to ThinkTank => PositionControl
                WAIT_COMMAND, //PathPlanner posts this to ThinkTank => PositionControl
                ACTIVITY_CHANGED, //Activity Control posts this to ThinkTank => PositionControl
                
                WALL_NO_LONGER_THREATENS_HOPPER, // CheckForHopperThreat(?) posts this to Tank=>HopperDrive
                WALL_THREATENS_HOPPER, // CheckForHopperThreat(?) posts this to Tank=>HopperDrive
                HIGH_POSITION_REACHED_RIGHT, // Encoder Interrupt posts this to Tank=>HopperDrive
                HIGH_POSITION_REACHED_LEFT, // Encoder Interrupt posts this to Tank=>HopperDrive
                DUMP_POSITION_REACHED, // Encoder Interrupt posts this to Tank=>HopperDrive
                LOW_POSITION_REACHED_RIGHT, // Encoder Interrupt posts this to Tank=>HopperDrive
                LOW_POSITION_REACHED_LEFT, // Encoder Interrupt posts this to Tank=>HopperDrive
                DEPOSIT_COMMAND, //PathPlanner posts this to Tank=>HopperDrive
                VCONTROL_UPDATE,
                SCONTROL_UPDATE,
                GO_TO_LOW_COMMAND,
                
                START_CALIBRATING_HOPPER, // keyboard posts to hopperdrive->calibrate mode
                RIGHT_ZEROED, // keyboard posts to hopperdrive->calibrate mode
                LEFT_ZEROED, // keyboard posts to hopperdrive->calibrate mode
                
                /****************************
                //--Old Activity State Commands--
                RASTER_COMMAND, //GameControl posts this to ThinkTank => ActivityControl
                CONTROL_RIGHT_WALL_COMMAND, //GameControl posts this to ThinkTank => ActivityControl
                CONTROL_LEFT_WALL_COMMAND, //GameControl posts this to ThinkTank => ActivityControl				    				    				    			    				    
				    DEPOSIT_IN_BIN_ONE_COMMAND, //GameControl posts this to ThinkTank => ActivityControl
				    DEPOSIT_IN_BIN_TWO_COMMAND, //GameControl posts this to ThinkTank => ActivityControl
				    DEPOSIT_IN_BIN_THREE_COMMAND, //GameControl posts this to ThinkTank => ActivityControl
				    DEPOSIT_IN_BIN_FOUR_COMMAND, //GameControl posts this to ThinkTank => ActivityControl
				    STAY_PUT_COMMAND, //GameControl posts this to ThinkTank => ActivityControl
				    *****************************/
				    
				    
				    ES_COMMAND_RECEIVED, //Interrupt posts this to QuerySM (SPI)
					//start IR/stepper events
				BEACON_BIN1_HONED,
				BEACON_BIN1_RIGHT,
				BEACON_BIN1_LEFT,
				BEACON_BIN1_RIGHT_LOST,
				BEACON_BIN1_LEFT_LOST,
				BEACON_BIN2_HONED,
				BEACON_BIN2_RIGHT,
				BEACON_BIN2_LEFT,
				BEACON_BIN2_RIGHT_LOST,
				BEACON_BIN2_LEFT_LOST,				
				BEACON_BIN3_HONED,
				BEACON_BIN3_RIGHT,
				BEACON_BIN3_LEFT,
				BEACON_BIN3_RIGHT_LOST,
				BEACON_BIN3_LEFT_LOST,				
				BEACON_BIN4_HONED,
				BEACON_BIN4_RIGHT,
				BEACON_BIN4_LEFT,
				BEACON_BIN4_RIGHT_LOST,
				BEACON_BIN4_LEFT_LOST,				
				NO_BEACON,
				STEP_CCW,
				STEP_CW,
				STOP_STEPPING,
				//TAKE_STEP,
				ZERO_FOUND,
				HONE_BIN1,
				HONE_BIN2,
				HONE_BIN3,
				HONE_BIN4,
				FIND_POSITION,
				THREE_FOUND,
				
				// late comers
				START_AVERAGING,
				STOP_AVERAGING,
				
				CALIBRATION_COMPLETE,
				BUTTON_PRESSED
				    } ES_EventTyp_t ;

/****************************************************************************/
// These are the definitions for the Distribution lists. Each definition
// should be a comma seperated list of post functions to indicate which
// services are on that distribution list.
#define NUM_DIST_LISTS 8
#if NUM_DIST_LISTS > 0 
#define DIST_LIST0 PostTankSM
#endif
#if NUM_DIST_LISTS > 1 
#define DIST_LIST1 PostThinkTankSM
#endif
#if NUM_DIST_LISTS > 2 
#define DIST_LIST2 PostTankSM, PostThinkTankSM, PostQuerySM, PostGameMonitorService
#endif
#if NUM_DIST_LISTS > 3 
#define DIST_LIST3 PostQuerySM
#endif
#if NUM_DIST_LISTS > 4 
#define DIST_LIST4 PostGameMonitorService
#endif
#if NUM_DIST_LISTS > 5 
#define DIST_LIST5 PostIRSM
#endif
#if NUM_DIST_LISTS > 6 
#define DIST_LIST6 PostStepperSM, PostIRSM
#endif
#if NUM_DIST_LISTS > 7 
#define DIST_LIST7 PostStepperSM
#endif

/****************************************************************************/
// This are the name of the Event checking funcion header file. 
#define EVENT_CHECK_HEADER "EventCheckers.h"

/****************************************************************************/
// This is the list of event checking functions 
#define EVENT_CHECK_LIST UpdateADPins, KeyStrokeEvents, CheckButtonEvents,Check4HopperStall, Check4HopperDelta, Check4Tape, Beacon_Bin1_Checker, Beacon_Bin2_Checker, Beacon_Bin3_Checker, Beacon_Bin4_Checker, AnyBeacons_Checker, Check4WallProximity, Check4BallsInPlay, Check4PositionFound, WallAngleChecker, BinChecker,  BallChecker

/****************************************************************************/
// These are the definitions for the post functions to be executed when the
// correspnding timer expires. All 8 must be defined. If you are not using
// a timers, then you can use TIMER_UNUSED
#define TIMER_UNUSED ((pPostFunc)0)
#define TIMER0_RESP_FUNC PostTankSM
#define TIMER1_RESP_FUNC PostThinkTankSM
#define TIMER2_RESP_FUNC PostQuerySM
#define TIMER3_RESP_FUNC PostGameMonitorService
#define TIMER4_RESP_FUNC PostThinkTankSM
#define TIMER5_RESP_FUNC PostThinkTankSM
#define TIMER6_RESP_FUNC PostTankSM
#define TIMER7_RESP_FUNC TIMER_UNUSED

/****************************************************************************/
// Give the timer numbers symbolc names to make it easier to move them
// to different timers if the need arises. Keep these definitons close to the
// definitions for the response functions to make it easier to check that
// the timer number matches where the timer event will be routed 

#define KEY_DOWN_TIMER 0
#define MULTI_PRESS_TIMER 1

#endif /* CONFIGURE_H */