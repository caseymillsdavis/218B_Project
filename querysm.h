/****************************************************************************
 
  Header file for template Flat Sate Machine 
  based on the Gen2 Events and Services Framework

 ****************************************************************************/

#ifndef Query_H
#define Query_H

// Event Definitions
#include "ES_Configure.h"
#include "ES_Types.h"
#include <stdio.h>
#include "EventCheckers.h"

// typedefs for the states
// State definitions for use with the query function
typedef enum { InitPState, QueryControl, UpdateFieldStatus } TemplateState_t ;


// Public Function Prototypes

boolean InitQuerySM ( uint8_t Priority );
boolean PostQuerySM( ES_Event ThisEvent );
ES_Event RunQuerySM( ES_Event ThisEvent );
//TemplateState_t QueryQuerySM ( void );

// Access Function Prototypes

unsigned int queryFSR ( int );

#endif /* FSMTemplate_H */

