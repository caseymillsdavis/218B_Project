#include <stdio.h>
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_Timers.h"
#include "MotorDrivers.h"
#include "CommandCom.h"
#include "EventCheckers.h"
#include "ME218_E128.h"
#include "GameConstants.h"
#include "ADS12.h"



void main( void) {
ES_Return_t ErrorType;


// When doing testing, it is usefull to annouce just which program
// is running.
puts("Starting Deaf Dumb and Blind Kid \n\r");
puts("using the 2nd Generation Events & Services Framework\n\r");

// Your hardware initialization function calls go here
motorDriveInit();

//initialize IR and stepper interrupts
InitInterrupts();

// initialize SPI module
CommandComInit();

// initialize Event Checkers
InitializeCheckers();

//initialize tape sensor
DDRU &= ~TAPE_PIN;

//initialize LEDs
DDRU |= RED_LED;
DDRU |= BLUE_LED;
PTU &= ~RED_LED;
PTU &= ~BLUE_LED;

//Initialize The AD Pins for Hopper Control Calibration

//PTAD&=(BIT0LO|BIT1LO);
DDRP&=BIT0LO;
ADS12_Init("OOOOOIAA"); 

// now initialize the Events and Services Framework and start it running
ErrorType = ES_Initialize(ES_Timer_RATE_32MS);
if ( ErrorType == Success ) {

  ErrorType = ES_Run();

}
//if we got to here, there was an error
switch (ErrorType){
  case FailedPointer:
    puts("Failed on NULL pointer");
    break;
  case FailedInit:
    puts("Failed Initialization");
    break;
 default:
    puts("Other Failure");
    break;
}
for(;;)
  ;

};

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/
