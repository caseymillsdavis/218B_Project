/****************************************************************************
 Template header file for Hierarchical Sate Machines AKA StateCharts
 02/08/12 adjsutments for use with the Events and Services Framework Gen2
 3/17/09  Fixed prototpyes to use Event_t
 ****************************************************************************/

#ifndef PlayingSM_H
#define PlayingSM_H


// typedefs for the states
// State definitions for use with the query function
typedef enum { JUST_PLAYING, STILL_PLAYING } TemplateState_t ;


// Public Function Prototypes

ES_Event RunPlayingSM( ES_Event CurrentEvent );
void StartPlayingSM ( ES_Event CurrentEvent );
TemplateState_t QueryPlayingSM ( void );

#endif /*SHMTemplate_H */

