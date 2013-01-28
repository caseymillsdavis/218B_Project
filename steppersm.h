/****************************************************************************
 
  Header file for template Flat Sate Machine 
  based on the Gen2 Events and Services Framework

 ****************************************************************************/

#ifndef Stepper_H
#define Stepper_H

// Event Definitions
#include "ES_Configure.h"
#include "ES_Types.h"
#include <stdio.h>
#include "EventCheckers.h"

// typedefs for the states
// State definitions for use with the query function
typedef enum { InitPState_Stepper, HoldingStepper, StepCCW, StepCW } StepperState_t ;


// Public Function Prototypes

boolean InitStepperSM ( uint8_t Priority );
boolean PostStepperSM( ES_Event ThisEvent );
ES_Event RunStepperSM( ES_Event ThisEvent );
//StepperState_t QueryStepperSM ( void );

// Access Function Prototypes
int accessStepCount(void);

#endif /* FSMTemplate_H */

