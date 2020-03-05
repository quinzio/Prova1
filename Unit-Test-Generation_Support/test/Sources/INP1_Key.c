/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INP1_Key.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INP1_Key.c
    \version see \ref FMW_VERSION 
    \brief Basic key function.
    \details Routines for key input operation and variables related managing.\n
    \note none
    \warning none
*/

/*PRQA S 0380, 0857 ++  #Compiler supports more than 4096 defines and more than 1024 macro definitions */

#ifdef _CANTATA_
#include "cantata.h"
#endif
#include "SPAL_Setup.h"
#include "SpalTypes.h"
#include "SpalBaseSystem.h"
#include MICRO_REGISTERS
#include SPAL_DEF
#include MAC0_Register  /* Needed for macro definitions */
#include "main.h"       /* Per strDebug */
#include "INP1_Key.h"

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

#ifdef _CANTATA_
#undef Nop
#define Nop()
#endif

#ifdef KEY_INPUT

/* -------------------------- Private Constants ----------------------------*/

/* -------------------------- Private typedefs ---------------------------- */

/* -------------------------- Private variables --------------------------- */
/** 
    \var u8KeyOffCount
    \brief Numbers of successive reading of key when key is off.
    \note This varaible is placed in \b DEFAULT .
    \warning This variable is \b static.
    \details None.
*/
  static u8 u8KeyOffCount = (u8)0;
  static u8 u8KeyOnCount  = (u8)0; 

/* -------------------------- Private functions --------------------------- */

/* --------------------------- Private macros ------------------------------ */

/*-----------------------------------------------------------------------------*/
/** 
    \fn void INP1_InitKey(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 21/03/2011
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Init key pin as input .  
    \details Init key pin as inputf.\n
                            \par Used Variables
    \return \b None.
    \note None.
    \warning None.
*/
void INP1_InitKey(void) /*PRQA S 3006 #Due to the necessary use of Nop function written in assembly code*/ /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u8 i;
  
  u8KeyOffCount = 0;
  u8KeyOnCount = 0;
  
  SET_KEY_ON_OFF_INPUT;
  
  Nop(); /*PRQA S 1006 #Necessary assembly code*/ /* To wait SET_KEY_ON_OFF_INPUT effect */
  Nop(); /*PRQA S 1006 #Necessary assembly code*/
  Nop(); /*PRQA S 1006 #Necessary assembly code*/
  
  for(i=(u8)0;i<MAX_KEY_OFF;i++)
  {
    (void)INP1_KeyIsOff();
  }
  
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void INP1_KeyIsOff(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 21/03/2011
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Checks if key is off.  
    \details Chekc if \ref TEST_KEY_ON_OFF is off.\n
                            \par Used Variables
    \return \b BOOL.
    \note None.
    \warning None.
*/
BOOL INP1_KeyIsOff(void)
{
  BOOL ret;

  if( TEST_KEY_ON_OFF )
  {
    u8KeyOnCount = 0;
    if( u8KeyOffCount >= MAX_KEY_OFF )
    {
      ret = (BOOL)TRUE;
    }
    else 
    {
      u8KeyOffCount++;
      ret = (BOOL)FALSE;
    }
  }
  else
  {
    u8KeyOffCount = 0;
    if( u8KeyOnCount >= MAX_KEY_OFF )
    {
      ret = (BOOL)FALSE;
    }
    else
    {
      u8KeyOnCount++;
      ret = (BOOL)TRUE;
    }
  }
  return (ret);
}

#endif  /*KEY_INPUT*/
/* END */

/*** (c) 2011 SPAL Automotive ****************** END OF FILE **************/
