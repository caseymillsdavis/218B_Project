#include <hidef.h>         /* common defines and macros */ 
#include <mc9s12e128.h>    /* derivative information */ 
#include <S12e128bits.h>   /* bit definitions  */ 
#include <Bin_Const.h>     /* macros to allow specifying binary constants */ 
#include <termio.h>        /* to get prototype for kbhit() */

void motorDriveInit(void);
void Drive(signed char);
void Rotate(float angle);