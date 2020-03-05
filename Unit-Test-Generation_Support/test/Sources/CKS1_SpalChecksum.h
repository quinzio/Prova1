/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  CKS1_SpalChecksum.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file CKS1_SpalChecksum.h
    \version see \ref FMW_VERSION 
    \brief Basic checksum functions.
    \details Routines for checksum computing.\n
    \note none
    \warning none
*/

#ifndef CHECKSUM_H
#define CHECKSUM_H


#include "CKS1_SpalChecksumArea.h"

/* Public constants -------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */
BOOL CKS_IsROMCheckSumOK(void);

#endif  /* CHECKSUM_H */

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
