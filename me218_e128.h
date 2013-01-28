/* last modified 2008.2.6 by KLG */
/* E128 header meta-file */

#ifndef _ME218_E128_H
#define _ME218_E128_H

#include <hidef.h>         /* common defines and macros */
#include <mc9s12e128.h>     /* derivative information */
#include <S12E128bits.h>    /* C32 bit definitions  */
#include <bitdefs.h>       /* BIT0HI, BIT0LO....definitions */

int kbhit(void);
/*
	functions to enable & disable interupts
	for CW 11/14/01
*/

#define disable() asm sei ;
#define enable()  asm cli ;
#endif
