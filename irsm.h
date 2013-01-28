/****************************************************************************
 
  Header file for template Flat Sate Machine 
  based on the Gen2 Events and Services Framework

 ****************************************************************************/

#ifndef IR_H
#define IR_H

// Event Definitions
#include "ES_Configure.h"
#include "ES_Types.h"
#include <stdio.h>
#include "EventCheckers.h"

// typedefs for the states
// State definitions for use with the query function
typedef enum { InitPState_IR, Holding, Honing_Bin1, Honing_Bin2,
			   Honing_Bin3, Honing_Bin4, Zeroing, Positioning } TemplateState_t ;


// Public Function Prototypes

boolean InitIRSM ( uint8_t Priority );
boolean PostIRSM( ES_Event ThisEvent );
ES_Event RunIRSM( ES_Event ThisEvent );
//TemplateState_t QueryIRSM ( void );

// Access Function Prototypes

float accessXPosition(void);
float accessYPosition(void);
float accessAngle(void);

#endif /* FSMTemplate_H */

