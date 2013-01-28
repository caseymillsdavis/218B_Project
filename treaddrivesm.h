/****************************************************************************
 Template header file for Hierarchical Sate Machines AKA StateCharts
 02/08/12 adjsutments for use with the Events and Services Framework Gen2
 3/17/09  Fixed prototpyes to use Event_t
 ****************************************************************************/

#ifndef TreadDrive_H
#define TreadDrive_H


// typedefs for the states
// State definitions for use with the query function
typedef enum { ROTATING, DRIVING, HOLDING } TreadState_t ;

// IF THE MOTORS STALL WHILE DRIVING THE COLLECTION, WE MIGHT WANT TO
// TRANSITION BACK TO A MORE USEFUL WAIT FOR ORDERS STATE AND GET
// OUT OF THIS STATE MACHINE - A DO-NOTHING STATE MIGHT BE USEFUL

// Public Function Prototypes

ES_Event RunTreadDriveSM( ES_Event CurrentEvent );
void StartTreadDriveSM ( ES_Event CurrentEvent );
TreadState_t QueryTreadDriveSM ( void );

#endif /*SHMTemplate_H */

