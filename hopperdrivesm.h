





/****************************************************************************
 Template header file for Hierarchical Sate Machines AKA StateCharts
 02/08/12 adjsutments for use with the Events and Services Framework Gen2
 3/17/09  Fixed prototpyes to use Event_t
 ****************************************************************************/

#ifndef HopperDrive_H
#define HopperDrive_H


// typedefs for the states
// State definitions for use with the query function
typedef enum { HOPPER_ACTIVE, HOPPER_INACTIVE, GOING_TO_HIGH_POSITION_NEITHERHIGH,
               GOING_TO_HIGH_POSITION_RIGHTHIGH, GOING_TO_HIGH_POSITION_LEFTHIGH, 
               GOING_TO_LOW_POSITION_NEITHERLOW,
               GOING_TO_LOW_POSITION_RIGHTLOW, GOING_TO_LOW_POSITION_LEFTLOW,
               DUMPING, HOPPER_ON, HOPPER_OFF, 
               LEFT_OFF_WAITING_FOR_RIGHT,
               WAIT_FOR_BOX_LOAD,
               DYNAMIC_CALIBRATION,
               HIGH_CALIBRATE_NEITHER,
               HIGH_CALIBRATE_RIGHT_STOPPED,
               HIGH_CALIBRATE_LEFT_STOPPED,
               WAIT_FOR_END_CALIBRATION
                } HopperState_t ;

// IF THE MOTORS STALL WHILE DRIVING THE COLLECTION, WE MIGHT WANT TO
// TRANSITION BACK TO A MORE USEFUL WAIT FOR ORDERS STATE AND GET
// OUT OF THIS STATE MACHINE - A DO-NOTHING STATE MIGHT BE USEFUL

// Public Function Prototypes

ES_Event RunHopperDriveSM( ES_Event CurrentEvent );
void StartHopperDriveSM ( ES_Event CurrentEvent );
HopperState_t QueryHopperDriveSM ( void );
int GetDelta(void);
int GetRightCount(void);
int GetLeftCount(void);
unsigned char GetPWMLeftHopper(void);
unsigned char GetPWMRightHopper(void);
int GetDeltaHistory(void);

void VelocityControl(void);
void StallControl(int);

#endif /*SHMTemplate_H */

