/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  CKS1_SpalChecksumArea.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file CKS1_SpalChecksumArea.h
    \version see \ref FMW_VERSION 
    \brief Basic checksum functions.
    \details Check sum area definition according to BLT version\n
    \note none
    \warning none
*/

#ifndef CHECKSUM_AREA_H
#define CHECKSUM_AREA_H

/* Public Constants -------------------------------------------------------- */

  #if (BOOTLOAD_VERS == BOOTLOADER_LESS_14)
      /* Non utilizzato */

  #elif (BOOTLOAD_VERS == BOOTLOADER_LATER_15)

      #define START_ADDRESS_AREA1_CRC_CCITT 0xC000
      #define END_ADDRESS_AREA1_CRC_CCITT   0xF761
      #define START_ADDRESS_AREA2_CRC_CCITT 0xFF68
      #define END_ADDRESS_AREA2_CRC_CCITT   0xFFFF
      #define ADDRESS_RESULT_CRC_CCITT      0xF764

  #elif (BOOTLOAD_VERS == BOOTLOADER_LATER_20)
      /* - 22/12/2015 - B.R. Allargata l'area di memoria dedcata all'applicativo perche' si e' */
      /* raggiunta l'occupazione di 20K dedicati */
#ifdef _CANTATA_
      #define START_ADDRESS_AREA1_CRC_CCITT &dummymem[0x0]
	  #define END_ADDRESS_AREA1_CRC_CCITT &dummymem[0xF]
	  #define START_ADDRESS_AREA2_CRC_CCITT &dummymem[0x10]
	  #define END_ADDRESS_AREA2_CRC_CCITT &dummymem[0x1F]
      #define ADDRESS_RESULT_CRC_CCITT      &dummymem[0xEF08]
      #define ADDRESS_CRC_CCITT_DEFINITION  &dummymem[0xEF00]
#else
      #define START_ADDRESS_AREA1_CRC_CCITT 0xA000
      #define END_ADDRESS_AREA1_CRC_CCITT   0xEEFF
      #define ADDRESS_AREA1_CRC_CCITT_FILL  0x3F
      #define START_ADDRESS_AREA2_CRC_CCITT 0xEF80
      #define END_ADDRESS_AREA2_CRC_CCITT   0xEFFF
      #define ADDRESS_RESULT_CRC_CCITT      0xEF08
      #define ADDRESS_CRC_CCITT_DEFINITION  0xEF00
#endif
  #endif /*BOOTLOAD_VERS*/

#endif
/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
