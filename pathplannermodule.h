#ifndef PathPlanner_H
#define PathPlanner_H

void PathPlanner(void);
boolean SafeToTravel(float Distance, float CurrentX, float CurrentY, float CurrentTHETA, int WallAngle);
boolean CurrentLocationIsSafe(float CurrentX, float CurrentY, float CurrentTHETA, int WallAngle);

float GetSeparationDistance(float GoalX,float GoalY,float CurrentX,float CurrentY);
float GetSeparationAngle(float CurrentPosition[3], float GoalPosition[2]);

void CheckPointsPlusPlus(void);
//Now a private function
//ES_Event GenerateNextStep_GoalPoint(float CurrentPosition[3], float GoalOne[2], float GoalTwo[2]);

#endif