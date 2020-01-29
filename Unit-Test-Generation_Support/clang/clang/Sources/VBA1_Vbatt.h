/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  VBA1_Vbatt.h

VERSION  :  1.2

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file VBA1_Vbatt.h
    \version see \ref FMW_VERSION 
    \brief Basic battery voltage functions prototypes.
    \details Routines for voltage operation and variables related initialization.\n
    \note none
    \warning none
*/

#ifndef VBATT_H
#define VBATT_H


/* Public Constants -------------------------------------------------------------------------------------------------- */
/* Constants for Vmon reading */
#if ( BATTERY_VOLTAGE == V_BATT_12V )
  #define MIN_OPERATING_VOLTAGE_VOLT      7.4     /* [V] */   /*  8.0 V da specifica JLR. */
  #define MAX_OPERATING_VOLTAGE_VOLT      17.2    /* [V] */   /* 16.5 V da specifica JLR. */
#else
  #define MIN_OPERATING_VOLTAGE_VOLT      14      /* [V] */
  #define MAX_OPERATING_VOLTAGE_VOLT      33      /* [V] */
#endif  /* BATTERY_VOLTAGE */

#define V_GAMMA_VDH_RVP_DIODE_PU          (u16)((float32_t)((float32_t)V_GAMMA_VDH_RVP_DIODE*(float32_t)VOLTAGE_RES_FOR_PU_BIT)/BASE_VOLTAGE_VOLT)  
#define V_DROP_VDHS_PU                    (u16)((float32_t)((float32_t)V_DROP_VDHS_VOLT*(float32_t)VOLTAGE_RES_FOR_PU_BIT)/BASE_VOLTAGE_VOLT)

#define VBUS_HYSTERESIS_LOW_VOLT          0.5     /* [V] */
#define VBUS_HYSTERESIS_LOW_PU            (u16)((float32_t)((float32_t)VBUS_HYSTERESIS_LOW_VOLT*(float32_t)VOLTAGE_RES_FOR_PU_BIT)/BASE_VOLTAGE_VOLT)

#define VBUS_HYSTERESIS_HIGH_VOLT         0.5     /* [V] */
#define VBUS_HYSTERESIS_HIGH_PU           (u16)((float32_t)((float32_t)VBUS_HYSTERESIS_HIGH_VOLT*(float32_t)VOLTAGE_RES_FOR_PU_BIT)/BASE_VOLTAGE_VOLT)

#ifdef RVP_SW_MANAGEMENT_ENABLE
  #define VBUS_HYSTERESIS_RVP_OFF         0.6       /* [V] - Diodo equivalente del circuito di RVP. */
  #define VBUS_HYSTERESIS_RVP_OFF_PU      (u16)((float32_t)((float32_t)VBUS_HYSTERESIS_RVP_OFF*(float32_t)VOLTAGE_RES_FOR_PU_BIT)/BASE_VOLTAGE_VOLT)
#endif  /* RVP_SW_MANAGEMENT_ENABLE */

#define MIN_OPERATING_VOLTAGE_PU          (u16)((float32_t)((float32_t)MIN_OPERATING_VOLTAGE_VOLT*(float32_t)VOLTAGE_RES_FOR_PU_BIT)/BASE_VOLTAGE_VOLT)
#define MAX_OPERATING_VOLTAGE_PU          (u16)((float32_t)((float32_t)MAX_OPERATING_VOLTAGE_VOLT*(float32_t)VOLTAGE_RES_FOR_PU_BIT)/BASE_VOLTAGE_VOLT)

#ifdef VBATT_FILTERING
  #define VBATT_MAX_OVERVOLTAGE_EVENTS    (u8)31
#else
  #define VBATT_MAX_OVERVOLTAGE_EVENTS    (u8)62
#endif  /* VBATT_FILTERING */

/* Public type definition -------------------------------------------------- */

/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */
void Vbatt_InitVbattOn (void);
void Vbatt_EnableVbattOn (void);
void Vbatt_DisableVbattOn (void);
BOOL Vbatt_CheckOverVoltage(void);
BOOL Vbatt_AutoTestVoltage(void);
u16  Vbatt_GetBusVoltage(void);
u16  Vbatt_GetVparam(void);
void Vbatt_SetDefaultParamV (void);
void Vbatt_InitOverVoltage(void);

#endif  /* VBATT_H */

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
