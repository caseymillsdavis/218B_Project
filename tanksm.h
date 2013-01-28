/****************************************************************************
 Template header file for Hierarchical Sate Machines AKA StateCharts

 ****************************************************************************/

#ifndef TankSM_H
#define TankSM_H

// State definitions for use with the query function
typedef enum { CALIBRATING_HOPPER, WAITING_FOR_START, PLAYING, DISABLED } TankState_t ;

// Public Function Prototypes

ES_Event RunTankSM( ES_Event CurrentEvent );
void StartTankSM ( ES_Event CurrentEvent );
boolean PostTankSM( ES_Event ThisEvent );
boolean InitTankSM ( uint8_t Priority );
TankState_t QueryTankSM(void);


#endif /*TopHSMTemplate_H */

