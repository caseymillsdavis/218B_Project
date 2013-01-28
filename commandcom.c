
/****************************************************************************
 Module
     CommandCom.c

 Revision
     1.0

 Description
     This is a module that implements SPI communication between the E128 and
     the ME218B project's Field Status Reporter.

 Notes
     

 History
 When           Who     What/Why
 -------------- ---     --------
 02/04/12 08:00 cmd     started
 02/18/12 03:15 cmd		updated for ME218B project
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
#include <hidef.h>         	/* common defines and macros */
#include <mc9s12e128.h>    	/* derivative information */ 
#include <S12e128bits.h>   	/* bit definitions */ 
#include <Bin_Const.h>		/* macros to allow specifying binary constants */
#include <termio.h>			/* to get prototype for kbhit() */
#include <stdio.h>			/* allow communcication with terminal */
#include "S12eVec.h"		/* bit definitions for E128 SPI */
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_PostList.h"

#include "CommandCom.h"		// does this need to be here?



/*----------------------------- Module Defines ----------------------------*/
#define NULL_COMMAND 		0x00
#define GENERATOR_READY		0xFF
#define BAUD_RATE_98		0x74
#define BAUD_RATE_49		0x75
#define BAUD_RATE_24		0x76
#define BAUD_RATE_12 		0x77
//#define BAUD_RATE_

/*---------------------------- Module Functions ---------------------------*/
void interrupt _Vec_spi CommandGenerator(void);


/*---------------------------- Module Variables ---------------------------*/
static unsigned char CurrentCommand = 0xFF;
static boolean ReadyForNewCommand = False;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     CommandComInit

 Parameters
     None. Could take a baud rate.

 Returns
     None.

 Description
 	 Initializes the SPI system on the E128. Configured in slave select
 	 output mode. Clock phase is set to latch bits on second edge of each SCK
 	 cycle. Clock polarity is set to active low (idle hi).
     
 Notes
     None.

 Author
     Casey Mills Davis, 01/04/12 10:40
****************************************************************************/



void CommandComInit(void)
{
	// Basic setup & operation
	SPICR1 |= _S12_SPE; 			// Enable SPI system	
	SPICR1 |= _S12_MSTR;			// Set to master mode
	
	// Controlling the SPI bit rate
	SPIBR = BAUD_RATE_24;			// Set baud rate
	
	// SPI clock control
	SPICR1 |= _S12_CPOL;			// Clock polarity active low
	SPICR1 |= _S12_CPHA;			// Clock phase - sample even edges
	
	// Slave select control
	SPICR1 |= _S12_SSOE;			// Slave select output enable
	SPICR2 |= _S12_MODFEN;			// Mode fault enable
	
	// SPI Interrupts
	SPICR1 |= _S12_SPIE;			// Enable SPI interrupts on SPIF flag
	
	//QueryCommandGenerator();
	//lastCommand = 0xFF;
	EnableInterrupts;
}


/****************************************************************************
 Function
     

 Parameters
     None.

 Returns
     None.
     
	 Command Byte Description of action 	
	 
	 0x00 	 	  Stop, hold Position, do not move or rotate 
	 0x02 	 	  Rotate Clockwise by 90 degrees 
	 0x03 	      Rotate Clockwise by 45 degrees
	 0x04 		  Rotate Counter-clockwise by 90 degrees
	 0x05 		  Rotate Counter-clockwise by 45 degrees
	 0x08 		  Drive forward half speed 
	 0x09 		  Drive forward full speed 
	 0x10 		  Drive in reverse half speed 
	 0x11 		  Drive in reverse full speed 
	 0x20 		  Align with beacon
	 0x40 		  Drive forward until tape detected

 Description
 	 Queries the Command Generator by writing 0xAA to it (by protocol).
     
 Notes
     None.

 Author
     Casey Mills Davis, 01/04/12 10:50
****************************************************************************/
#ifndef DUMMYCOMMANDTEST

void FieldStatusCommand(char command)
{
	(void) SPISR;
	SPIDR = command;
}

void GetCommand(void)
{
	(void) SPISR;
	SPIDR = NULL_COMMAND;
}

void interrupt _Vec_spi CommandGenerator(void)
{
	
	ES_Event ThisEvent;	
	(void) SPISR;					// Clear SPIF flag
	CurrentCommand = SPIDR;			// Read command
  
	ThisEvent.EventType = ES_COMMAND_RECEIVED;
	ThisEvent.EventParam = CurrentCommand;
	ES_PostList03(ThisEvent);
}

#endif