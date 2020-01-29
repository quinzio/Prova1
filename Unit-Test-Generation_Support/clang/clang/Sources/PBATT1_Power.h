/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  PBATT1_Power.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file PBATT1_Power.c
    \version see \ref FMW_VERSION 
    \brief Basic structures and prototypes for Pbatt management.
    \note none
    \warning none
*/

#ifndef POWER_H
#define POWER_H  


/* Public type definition -------------------------------------------------- */
typedef struct 
 {
  u16 u16VbattPU;
  u16 u16IbattPU;
  u32 u32PbattPU;
} structPower;

/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */
void Power_InitPowerStruct(void);
void Power_UpdatePowerStruct(void);
void Power_Set_structPowerMeas_u16VbattPU(u16 value);
void Power_Set_structPowerMeas_u16IbattPU(u16 value);
u16  Power_Get_structPowerMeas_u16VbattPU(void);
u16  Power_Get_structPowerMeas_u16IbattPU(void);
u32  Power_Get_structPowerMeas_u32PbattPU(void);

#endif  /* POWER_H */
/*** (c) 2015 SPAL Automotive ****************** END OF FILE **************/
