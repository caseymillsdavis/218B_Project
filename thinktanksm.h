/****************************************************************************
 Template header file for Hierarchical State Machines AKA StateCharts
 
 "ThinkTank" HSM

 ****************************************************************************/

#ifndef ThinkTankSM_H
#define ThinkTankSM_H

// State definitions for use with the query function
typedef enum { GAMBIT, MAIN_STRATEGY_ONE, ENDGAME } ThinkTankState_t ;

// Public Function Prototypes

ES_Event RunThinkTankSM( ES_Event CurrentEvent );
void StartThinkTankSM ( ES_Event CurrentEvent );
boolean PostThinkTankSM( ES_Event ThisEvent );
boolean InitThinkTankSM ( uint8_t Priority );
void UpdateDisplay(void);

#endif /*ThinkTankSM_H */

