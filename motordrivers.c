#include <hidef.h>         /* common defines and macros */ 
#include <mc9s12e128.h>    /* derivative information */ 
#include <S12e128bits.h>   /* bit definitions  */ 
#include <Bin_Const.h>     /* macros to allow specifying binary constants */ 
#include <termio.h>        /* to get prototype for kbhit() */
#include <stdio.h>

#define ROTATION_SPEED 100 //if rotation speed is not 50 or 100 need to add a case to motorA/BDrive w/ corresponding duty cycle
#define ROTATION_SPEED_LEFT 88

void LeftMotorDrive(signed char);
void RightMotorDrive(signed char);

void motorDriveInit(void)
{
   DDRU|=0b00001111;  //set u ports 0,1,2,3 as outputs - ports 0 and 1 = clockSA, 2 and 3 = clockSB
   PWME=0b1111;  //enable ports PWME0, PWME1, PWME2, PWME3
   PWMPRCLK &= ~(0b1110111); //clear bits 0 through 3
   //PWMPRCLK=0b0; //prescale both clocks to 24MHz
   PWMSCLA=0b11; //set clock A to 4MHz = 24/(3*2)
   PWMSCLB=0b1111000; //set clockB to 0.5MHz = 500kHz = 24/(24*2)
   PWMPOL=0b1111;  //set polarity of channel PWME0, PWME1 to high
   PWMCLK=0b1111;  //use the scaled clock values at all pwm ports
   PWMCAE=0b000000; //no center alignment
   MODRR=0b00001111; //map to U ports 0, 1, 2, and 3
   PWMPER0=100; //set the final rate to 40kHz
   PWMPER1=100; //set the final rate to 40kHz
   PWMPER2=100; //set the final rate to 2.5kHz
   PWMPER3=100; //set the final rate to 2.5kHz
   PWMDTY0=0;
   PWMDTY1=0;
   PWMDTY2=0;
   PWMDTY3=0;
   
   DDRE|= (BIT7HI);
   DDRT|= (BIT7HI);
   //PTU &= (BIT5LO&BIT4LO);
}

void Drive(signed char speed)
{
signed char CompensatedSpeed;
if (speed == 0)
{
   CompensatedSpeed = 0;   
} else
{
   CompensatedSpeed = (signed char)((int)((speed*90)/100));
   //printf("cspd: %d\n\r",CompensatedSpeed);
}



LeftMotorDrive(CompensatedSpeed);
//PORTE |=BIT7HI; 
//PWMDTY0=50;
//PTT |=BIT7HI; 
//PWMDTY1=50; 
RightMotorDrive(-1*speed);
}

void Rotate(float angle)
{

//unsigned int duration;
if(angle>0)
{
LeftMotorDrive(-1*ROTATION_SPEED_LEFT);
RightMotorDrive(-1*ROTATION_SPEED);
}else
{
angle=-1*angle;
LeftMotorDrive(ROTATION_SPEED_LEFT);
RightMotorDrive(ROTATION_SPEED);
}
//duration=(100 * (int)angle) / 9;//Adjust 1000 to the time it takes to rotate 90 degrees
//printf("the duration is: %u\r\n", duration);
//ES_Timer_InitTimer(ROTATION_TIMER,duration);
}

void LeftMotorDrive(signed char speed)
{

   static signed char DutyCycle;// = 100;
   //printf("Left speed is %d\n\r",speed);
   
   if (speed > 0)
   {
      PORTE |=BIT7HI; 
      DutyCycle = ((speed*86)/100 + 14);  
   }else if (speed == 0)
   {
      DutyCycle = 0;   
   } else
   {
      PORTE &=BIT7LO;
      speed = -1*speed;
      DutyCycle = ((speed*86)/100 + 14);  
   }
      
   
   //printf("Left Duty Cycle is %d\n\r",DutyCycle);
      
   PWMDTY0=DutyCycle;
      

}

void RightMotorDrive(signed char speed)
{

   static signed char DutyCycle;// = 100;
   //printf("Right speed is %d\n\r",speed);
   
   if (speed > 0)
   {
      PTT |=BIT7HI;
      DutyCycle = ((speed*86)/100 + 14);   
   }else if (speed == 0)
   {
      DutyCycle = 0;   
   } else
   {      
      PTT &=BIT7LO;
      speed = -1*speed;  
      DutyCycle = ((speed*86)/100 + 14);
   }
      
   //printf("Right Duty Cycle is %d.\n\r",DutyCycle);
      
   PWMDTY1=DutyCycle;

}