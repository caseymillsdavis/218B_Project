/****************************************************************************
 
  Header file for template service 
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef GameMonitor_H
#define GameMonitor_H

#include "ES_Types.h"

#define GAMEMONITORTIMER 4
#define LONG_TIME 6000

#define PI 3.14159265

// Public Function Prototypes

boolean InitGameMonitorService ( uint8_t Priority );
boolean PostGameMonitorService( ES_Event ThisEvent );
ES_Event RunGameMonitorService( ES_Event ThisEvent );

void UpdateCurrentPosition(void);

void UpdateWallPoints(void);

void UpdateWallLine(void);

float GetX(void);
float GetY(void); 
float GetTheta(void); 
unsigned int GetWallAngle(void);
float GetWallSlope(void);
float GetWallIntercept(void);
float WallStagingPointX(boolean isRight);
float WallStagingPointY(boolean isRight); 
float WallFirstPointX(void);
float WallFirstPointY(void);
float PushPointX(void);
float PushPointY(void);



#endif /* ServTemplate_H */

