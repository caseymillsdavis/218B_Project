/****************************************************************************
 Module
     ES_CheckEvents.h
 Description
     header file for use with the data structures to define the event checking
     functions and the function to loop through the array calling the checkers
 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 02/08/12 15:27 jec      added #define for the case of no user event checkers
 01/15/12 12:00 jec      new header for local types
 10/16/11 17:17 jec      started coding
*****************************************************************************/

#ifndef ES_CheckEvents_H
#define ES_CheckEvents_H

#include "ES_Types.h"

typedef boolean CheckFunc( void );

typedef CheckFunc (*pCheckFunc);

boolean ES_CheckUserEvents( void );

#define NO_EVENT_CHECKERS ((pCheckFunc)0)

#endif  // ES_CheckEvents_H