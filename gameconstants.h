//Global Variables
extern COUNTS_TO_HIGH;
extern HOPPER_OFFSET;

#ifndef GAME_CONSTANTS_H
#define GAME_CONSTANTS_H

#define PROPORTIONAL_CONSTANT 3

#define INFINITY 3000 // could be as low as 30 or 40
#define NO_INTERCEPT 3000 // could be as low as 4*12*INFINITY

#define ZERO_SPEED 0
#define GENTLE_FORWARD 90

#define DEBOUNCE_TIME 1


#define ONE_SECOND 31
#define TWO_MINUTES 3662
#define STALL_TIME 3
#define THIRTY_SECONDS 915
#define TEN_SECONDS 10*ONE_SECOND
#define DISPLAY_REFRESH_TIME 10
#define FSR_COMMAND_TIME	2 
#define TIME_NEEDED_TO_COMPLETE_A_DEPOSIT TEN_SECONDS
#define TENTH_OF_A_SECOND 3


#define MAIN_GAME_TIMER 0
#define THINKTANKTIMER 1
#define FSR_TIMER 2
#define GAME_MONITOR_TIMER 3
#define DEPOSIT_COMPLETE_TIMER 4
#define DEPOSIT_TIME_TIMER 5
#define MOVEMENT_TIMER 6



#define WAIT_FOR_POSITION_FOUND 4
#define WAIT_FOR_DEPOSIT_COMPLETE 11
#define WAIT_FOR_A_TENTH_SECOND 3
#define WAIT_FOR_A_WIGGLE_TIME 8

#define LEFT 0
#define RIGHT 1
#define BOTH 2

//#define PROPORTIONAL_CONSTANT 4
#define INTEGRAL_CONSTANT 10
#define LEFT_OR_REAR_HOPPER_PWM PWMDTY3
#define RIGHT_OR_FRONT_HOPPER_PWM PWMDTY2

#define STEP_DISTANCE 485
#define POSITION_ERROR 1
#define ANGLE_ERROR 3
#define CONVERT_TIME_TO_ANGLE 1.8  //(180/3.2*.032) // degrees/clock counts (32 MS)
#define CONVERT_TIME_TO_DISTANCE 0.194  //(24.25/125) // EMPIRICAL VALUE inches/clock count (32 MS)
#define CONVERT_ANGLE_TO_TIME (1/1.8) // 3.2/(180*.032) clock counts/degrees
#define CONVERT_DISTANCE_TO_TIME 5.15463918  // 1/(6.06*.032) clock counts/inch


#define RHR_CORRECT -1 // correct for right hand rule vs. clockwise

#define OUTER_RADIUS 48
#define SAFE_DISTANCE_FROM_WALL 9
#define RADIUS_OF_SAFETY (OUTER_RADIUS - SAFE_DISTANCE_FROM_WALL)
#define HALF_BOT_LENGTH 6

#define StagingPointForBinOneX (48 + 16.9705627)//12.7279221)//25.456)
#define StagingPointForBinOneY (48 + 16.9705627)//12.7279221)//25.456)
#define StagingPointForBinOneTHETA (-45)

#define StagingPointForBinTwoX (48 + 16.9705627)//12.7279221)//25.456)
#define StagingPointForBinTwoY (48 - 16.9705627)//12.7279221)//25.456)
#define StagingPointForBinTwoTHETA (-135)

#define StagingPointForBinThreeX (48 - 16.9705627)//12.7279221)//25.456)
#define StagingPointForBinThreeY (48 - 16.9705627)//12.7279221)//25.456)
#define StagingPointForBinThreeTHETA (135)

#define StagingPointForBinFourX (48 - 16.9705627)//12.7279221)//25.456)
#define StagingPointForBinFourY (48 + 16.9705627)//12.7279221)//25.456)
#define StagingPointForBinFourTHETA (45)

#define DepositPointForBinOneX (48 + 31)//29.6984848)//25.456)
#define DepositPointForBinOneY (48 + 31)//29.6984848)//25.456)
#define DepositPointForBinOneTHETA (-45)

#define DepositPointForBinTwoX (48 + 31)//29.6984848)//25.456)
#define DepositPointForBinTwoY (48 - 31)//29.6984848)//25.456)
#define DepositPointForBinTwoTHETA (-135)

#define DepositPointForBinThreeX (48 - 31)//29.6984848)//25.456)
#define DepositPointForBinThreeY (48 - 31)//29.6984848)//25.456)
#define DepositPointForBinThreeTHETA (135)

#define DepositPointForBinFourX (48 - 31)//29.6984848)//25.456)
#define DepositPointForBinFourY (48 + 31)//29.6984848)//25.456)
#define DepositPointForBinFourTHETA (45)

#define BinOneDepositX (48 + 28.284) 
#define BinOneDepositY (48 + 28.284)

#define BinTwoDepositX (48 + 28.284)
#define BinTwoDepositY (48 - 28.284)

#define BinThreeDepositX (48 - 28.284)
#define BinThreeDepositY (48 - 28.284)

#define BinFourDepositX (48 - 28.284)
#define BinFourDepositY (48 + 28.284)

//#define RUN_HOPPER_CALIBRATION_ROUTINE

// IRSM constants
#define BIN1 1
#define BIN2 2
#define BIN3 3
#define BIN4 4
#define BIN_1_CODE 0
#define BIN_2_CODE 3
#define BIN_3_CODE 2
#define BIN_4_CODE 1
#define BAD_BIN 5

#define LENGTH 94
#define PI 3.14159265
#define DEG_TO_RAD * (PI / 180)
#define RAD_TO_DEG * (180 / PI)

#define HALF_STEP_MULTIPLIER 0.45
#define MICRO_STEP_MULTIPLIER 0.1125 

// StepperSM constants
// 375 will be 2ms using a 128 divisor
#define SPEED 375	
#define DIR_PIN 	BIT2HI
#define STEP_PIN  BIT3HI
#define MS2_PIN   BIT4HI

// LED constants
#define RED_LED     BIT6HI
#define BLUE_LED    BIT7HI

// Tape constant
#define TAPE_PIN BIT4HI

#define INIT_BALLS 	0
// these are defined the same above for beacons
// these were for use with the FSR
//#define BIN1		1
//#define BIN2		2
//#define BIN3		3
//#define BIN4		4
#define WALL_ANGLE	5

// originally this was set to 20
#define BEACON_ERROR 50

//AD Calibration constants
#define PROP_THRESHOLD 10
#define OFFSET_THRESHOLD 10
 
#endif