/****************************************************************************
 Template header file for Hierarchical Sate Machines AKA StateCharts

 ****************************************************************************/

#ifndef MW_MapKeys_H
#define MW_MapKeys_H

// Public Function Prototypes

boolean InitMapKeys ( uint8_t Priority );
boolean PostMapKeys( ES_Event ThisEvent );
ES_Event RunMapKeys( ES_Event ThisEvent );


#endif /*MW_MapKeys_H */

