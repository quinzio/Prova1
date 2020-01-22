/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  PARAM1_Param.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/**
    \file PARAM1_Param.h
    \version see \ref FMW_VERSION 
    \brief Trimming parameter locations.
    \details none
    \note none
    \warning none
*/

#ifndef PARAM_H
#define PARAM_H


/* Public Constants -------------------------------------------------------- */

/* Trimming parameters */
#if (BOOTLOAD_VERS == BOOTLOADER_LESS_14)
  /* PARAMETER ADDRESS */
  #define I_PARAM_ADDRESS         @0x00F770
  #define V_PARAM_ADDRESS         @0x00F772
  #define T_PARAM_ADDRESS         @0x00F774
  #define T_MOS_PARAM_ADDRESS     @0x00F778
  #define V_ANALOG_PARAM_ADDRESS  @0x00F77A
  #define NTC_PARAM_ADDRESS       @0x00F77B

  #define PARAM_INIT              ((u8*)0x00F770)
  #define PARAM_END               ((u8*)0x00F77F)

#elif (BOOTLOAD_VERS == BOOTLOADER_LATER_15)
  /* PARAMETER ADDRESS */
  #define I_PARAM_ADDRESS         @0x000400
  #define V_PARAM_ADDRESS         @0x000402
  #define T_PARAM_ADDRESS         @0x000404
  #define T_MOS_PARAM_ADDRESS     @0x000408
  #define V_ANALOG_PARAM_ADDRESS  @0x00040A
  #define NTC_PARAM_ADDRESS       @0x00040B
  #define PWM_TO_AN_PARAM_ADDRESS @0x00040C

  #define PARAM_CHECKSUM_ADDRESS  @0x000410
  #define LOCK_BOOTL_ADDRESS      @0x00F77F

  #define PARAM_INIT              ((u8*)0x000400)
  #define PARAM_END               ((u8*)0x000410)

  #define DATAMATRIX_INIT         ((u8*)0x000420)
  #define DATAMATRIX_END          ((u8*)0x00042B)

  #define BTL_VERS_ADDR ((u8*)0x00FF9F)
  #define INTERRUPT_VECTOR_TABLE_ADDRESS @0xFF80U   /* Indirizzo tabella degli interrupt */
  #define INTERRUPT_VECTOR_RESET_ADDRESS @0xFFFAU   /* Indirizzo tabella degli interrupt - ultimi 3 vettori di reset */

#elif (BOOTLOAD_VERS == BOOTLOADER_LATER_20)
  /* PARAMETER ADDRESS */
#ifndef _CANTATA_
  #define I_PARAM_ADDRESS         @0x000400 /*PRQA S 0289 #mandatory for linker behavior*/
  #define V_PARAM_ADDRESS         @0x000402
  #define T_PARAM_ADDRESS         @0x000404
  #define T_MOS_PARAM_ADDRESS     @0x000408
  #define V_ANALOG_PARAM_ADDRESS  @0x00040A
  #define NTC_PARAM_ADDRESS       @0x00040B
  #define PWM_TO_AN_PARAM_ADDRESS @0x00040C
#else
  #define I_PARAM_ADDRESS
  #define V_PARAM_ADDRESS
  #define T_PARAM_ADDRESS
  #define T_MOS_PARAM_ADDRESS
  #define V_ANALOG_PARAM_ADDRESS
  #define NTC_PARAM_ADDRESS
  #define PWM_TO_AN_PARAM_ADDRESS
#endif

  #ifdef  GM_LIN_ENCODING
    /* GM Read By Identifier Parameters */
    #define SERIAL_NUMBER_ADDRESS  @0x000430
    #define SERIAL_NUMBER_SIZE 4
  #endif  /* GM_LIN_ENCODING */

#ifndef _CANTATA_
  #define PARAM_CHECKSUM_ADDRESS  @0x000410
  #define LOCK_BOOTL_ADDRESS      @0x00EF0AU

  #define PARAM_INIT ((u8*)0x000400)
  #define PARAM_END  ((u8*)0x000410)

  #define DATAMATRIX_INIT ((u8*)0x000420)
  #define DATAMATRIX_END  ((u8*)0x00042B)

  #define BTL_VERS_ADDR ((u8*)0x00FF9FU) 
  #define INTERRUPT_VECTOR_TABLE_ADDRESS @0xEF80U  /* Indirizzo tabella degli interrupt */
  #define INTERRUPT_VECTOR_RESET_ADDRESS @0xEFFAU  /* Indirizzo tabella degli interrupt - ultimi 3 vettori di reset */
#else
  #define PARAM_CHECKSUM_ADDRESS
  #define LOCK_BOOTL_ADDRESS

  #define PARAM_INIT ((u8*)dummymem + 0x000400)
  #define PARAM_END  ((u8*)dummymem + 0x000410)

  #define DATAMATRIX_INIT ((u8*)dummymem + 0x000420)
  #define DATAMATRIX_END  ((u8*)dummymem + 0x00042B)

  #define BTL_VERS_ADDR ((u8*)dummymem + 0x00FF9FU)
  #define INTERRUPT_VECTOR_TABLE_ADDRESS
  #define INTERRUPT_VECTOR_RESET_ADDRESS
#endif

  #ifdef BMW_LIN_ENCODING
    /* BMW Read By Identifier Parameters */
    #define YEAR_PRODUCTION_ADDRESS             @0x000434          /* see "I:\IPMOEL\Bootloader\Occupazione EEPROM S12GXXX\Occupazione EEPROM S12GN16 S12GN32_01.xlsm" */
    #define MONTH_PRODUCTION_ADDRESS            @0x000435
    #define DAY_PRODUCTION_ADDRESS              @0x000436
    #define SERIAL_NUMBER_ADDRESS               @0x000430
    #define HW_REFERERENCE_NUMBER_ADDRESS       @0x000438
    #define SPAL_HW_REFERERENCE_NUMBER_ADDRESS  @0x00043A
    #define SERIAL_NUMBER_SIZE 4  
  #endif  /* BMW_LIN_ENCODING */

#endif  /* BOOTLOAD_VERS */

#ifdef BOOTLOADER_UNLOCK
 #define BOOTLOADER_LOCK_VALUE ((u8)0xFF)
#endif

#ifdef BOOTLOADER_LOCK  
 #define BOOTLOADER_LOCK_VALUE ((u8)0xF0)
#endif

/* RAM addressing */
#define RAM_INIT                              0x3C00        /* As defined into project.prm */
#define RAM_END                               0x3FFF        /* As defined into project.prm */

#endif /* PARAM_H */

/*** (c) 2016 SPAL Automotive ****************** END OF FILE **************/
