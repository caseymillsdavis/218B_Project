// prototypes for test event checkers

boolean Check4BallsInPlay(void);
boolean BallChecker(void);
boolean BinChecker(void);
boolean WallAngleChecker(void);
void InitializeCheckers(void); // merge
boolean Check4PositionFound(void);
boolean Check4HopperDelta(void);
boolean Check4HopperStall(void);
boolean Beacon_Bin1_Checker(void);
boolean Beacon_Bin2_Checker(void);
boolean Beacon_Bin3_Checker(void);
boolean Beacon_Bin4_Checker(void);
boolean Check4Tape(void);
boolean AnyBeacons_Checker(void);
void InitInterrupts(void);
boolean Check4WallProximity(void);
boolean UpdateADPins(void);
boolean CheckButtonEvents(void);

#include <bitdefs.h>

boolean KeyStrokeEvents(void); // merge w/keystroke events


// #defines that go with the Event Checkers

#define LOCK_PIN    BIT0HI
#define UNLOCK_PIN  BIT1HI


