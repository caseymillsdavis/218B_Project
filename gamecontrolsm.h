/****************************************************************************
 Template header file for Hierarchical Sate Machines AKA StateCharts
 02/08/12 adjsutments for use with the Events and Services Framework Gen2
 3/17/09  Fixed prototpyes to use Event_t
 ****************************************************************************/

#ifndef GameControl_H
#define GameControl_H


// typedefs for the states
// State definitions for use with the query function
typedef enum { JUST_RASTER, DECIDE_WALL, DECIDE_BIN, DEPOSIT, PUSH_WALL,
               FIND_SIDE } GameState_t ;
               
typedef enum { RASTER, CONTROL_LEFT_WALL, CONTROL_RIGHT_WALL,
			      DEPOSIT_IN_BIN_ONE, DEPOSIT_IN_BIN_TWO, DEPOSIT_IN_BIN_THREE,
			      DEPOSIT_IN_BIN_FOUR, STAY_PUT} ActivityState_t ;               


// Public Function Prototypes

ES_Event RunGameControlSM( ES_Event CurrentEvent );
void StartGameControlSM ( ES_Event CurrentEvent );
GameState_t QueryGameControlSM ( void );
ActivityState_t QueryActivityControlSM ( void );

#endif /*SHMTemplate_H */

