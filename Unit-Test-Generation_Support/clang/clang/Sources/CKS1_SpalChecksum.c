/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  CKS1_SpalChecksum.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file CKS1_SpalChecksum.c
    \version see \ref FMW_VERSION 
    \brief Basic checksum functions.
    \details Routines for checksum computing.\n
    \note none
    \warning none
*/

/*PRQA S 0380, 0857 ++  #Compiler supports more than 4096 defines and more than 1024 macro definitions */

#ifdef _CANTATA_
#include "cantata.h"
#endif
#include "checksum.h"
#include "SPAL_Setup.h"
#include "SpalTypes.h"
#include "SpalBaseSystem.h"
#include MICRO_REGISTERS
#include SPAL_DEF
#include MAC0_Register
#include "WTD1_Watchdog.h"
#include "CKS1_SpalChecksum.h"

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

/* ---------------------------Private functions-------------------------------*/

#if (BOOTLOAD_VERS == BOOTLOADER_LATER_20)

/*PRQA S 289, 0306, 1019, 1281, 3218 ++ #mandatory for linker behavior*/
#define N_MEM_AREAS_APL 2  /* Total number of application memory areas present in const struct __ChecksumArea areas[] */

static const struct __ChecksumArea apl_areas[] = {
   {(_CHECKSUM_ConstMemBytePtr)(START_ADDRESS_AREA1_CRC_CCITT), ((END_ADDRESS_AREA1_CRC_CCITT-START_ADDRESS_AREA1_CRC_CCITT)+1u)},
   {(_CHECKSUM_ConstMemBytePtr)(START_ADDRESS_AREA2_CRC_CCITT), ((END_ADDRESS_AREA2_CRC_CCITT-START_ADDRESS_AREA2_CRC_CCITT)+1u)}
};

#ifdef _CANTATA_
static const volatile u16 *VectCheckSumAddress[4] =
#else
static const volatile u16 VectCheckSumAddress[4] @ADDRESS_CRC_CCITT_DEFINITION = 
#endif
{ START_ADDRESS_AREA1_CRC_CCITT,
    END_ADDRESS_AREA1_CRC_CCITT,
  START_ADDRESS_AREA2_CRC_CCITT,
    END_ADDRESS_AREA2_CRC_CCITT
};
/*PRQA S 289, 0306, 1019, 1281, 3218 -- */

#endif


 #ifdef CHECKSUM_SPAL_ENABLED

/** 
    \fn BOOL CKS_IsROMCheckSumOK(void) 

    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>


    \date 13/10/2011
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief The function performs a simply ROM check  
    
    \details Checksum
    After BOOTLOADER 20 ROM checksum must be perfomed into application
    Checksum method is CHECKSUM_STORAGE_CRC_CCITT (see spal.prm file into CHECKSUM section)
   
    \return \b bool Returns TRUE when Checksum result is correct
 */

/*PRQA S 0306, 1281 ++ #mandatory for linker behavior*/
BOOL CKS_IsROMCheckSumOK(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  BOOL ret;

  if(_Checksum_CheckAreasCRC_CCITT(apl_areas, N_MEM_AREAS_APL, 0xFFFFu) == (*(u16*)ADDRESS_RESULT_CRC_CCITT))
  {
    ret = (BOOL)TRUE;
  }
  else
  {
    ret = (BOOL)FALSE;
  }

  return (ret);
}
/*PRQA S 0306, 1281 --*/
#endif

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
