/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  ADM0_ADCS12.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file ADM0_ADCS12.h
    \version see \ref FMW_VERSION 
    \brief ADC Conversion routines Prototypes
    \details This routines and variables manage ADC convertion.\n
    \note none
    \warning none
*/

#ifndef ADM0_ADC_H
#define ADM0_ADC_H  


/* Public Constants -------------------------------------------------------- */
/* A/D resolution */
#define AD_MAXIMUM_RES_SHIFT           (u16)10
#define AD_MAXIMUM_RES_BIT             (u16)((u16)((u16)1<<AD_MAXIMUM_RES_SHIFT) - (u16)1)
#define OFFSET_SHIFT                   15

/* Public type definition -------------------------------------------------- */

/* Public Variables -------------------------------------------------------- */
#pragma DATA_SEG SHORT _DATA_ZEROPAGE
extern volatile u16 buffu16ADC_READING[ADC_READING_LENGHT];

#pragma DATA_SEG DEFAULT
/* extern const u8 buffu8ADC_Converting[ADC_READING_LENGHT]; */
extern u8 u8InverterStatus;
 
/* Public Functions prototypes --------------------------------------------- */

#endif  /* ADM0_ADC_H */

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
