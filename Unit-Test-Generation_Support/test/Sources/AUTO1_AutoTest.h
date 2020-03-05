/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  AUTO1_AutoTest.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file AUTO1_AutoTest.h
    \version see \ref FMW_VERSION 
    \brief Initial drive test functions and related parameter constants.
    \details Test functions for intial readings of bus current, bus voltage and TLE temperature\n
    \note none
    \warning none
*/


#ifndef AUTOTEST_H
#define AUTOTEST_H


/* Public Constants -------------------------------------------------------- */
/* Autotest parameters */
#define AUTOTEST_HIBUS_THRESHOLD  (u8)150
#define AUTOTEST_LOBUS_THRESHOLD  (u8)10

#define AUTOTEST_HIBATT_THRESHOLD (u8)200
#define AUTOTEST_LOBATT_THRESHOLD (u8)10

#define AUTOTEST_LVBUS_THRESHOLD  (u8)34         /* Means 5 V on bus voltage monitor (7.5 kOhm and 47 kOhm) */
#define AUTOTEST_HVBUS_THRESHOLD  (u8)0xFA       /* ADC Channel selection Mask */

#define AUTOTEST_TLE_TAMB_LOW_THRESHOLD            (u8)200      /* Means: -75°C. Means: 1V. Datasheet specifies 1.15V  */
#define AUTOTEST_TLE_TAMB_HIGH_THRESHOLD           (u16)475     /* Means: 200°C. Means: 1.9V. Datasheet specifies 1.8V */
#if defined(MOSFET_OVERTEMP_BAV99)                 /* Diode */
  #define AUTOTEST_MOS_TEMPERATURE_HIGH_THRESHOLD  (u16)400     /* Means -157°C */
  #define AUTOTEST_MOS_TEMPERATURE_LOW_THRESHOLD   (u16)61      /* Means 250°C  */
#elif defined(MOSFET_OVERTEMP_NCP18XH103F)         /* Murata NTC */
  #define AUTOTEST_MOS_TEMPERATURE_HIGH_THRESHOLD  (u16)1020    /* -55°C !!! threshold near to nominal low limit */
  #define AUTOTEST_MOS_TEMPERATURE_LOW_THRESHOLD   (u16)30      /* 200°C                                         */
#endif

/* Parameter tolerances */
  #define I_PARAM_REF               (u16)32768
  #define I_PARAM_TOL               (u16)4915       /* 15% of I_PARAM_REF */
  #define V_PARAM_REF               (u16)32768
  #define V_PARAM_TOL               (u16)3277       /* 10% of V_PARAM_REF */
  #define T_PARAM_REF               (u16)300
  #define T_PARAM_TOL               (u16)30         /* 10% of T_PARAM_REF */
  #define T_MOS_PARAM_REF           (u16)247
  #define T_MOS_PARAM_TOL           (u16)25         /* 10% of T_MOS_PARAM_REF */
  #define AN_PARAM_REF              (u16)128
  #define AN_PARAM_TOL              (u16)18         /* 15% of AN_PARAM_REF */
  #define PWM_TO_AN_PARAM_REF       (u16)32768
  #define PWM_TO_AN_PARAM_TOL       (u16)3277       /* 10% of PWM_TO_AN_PARAM_REF */

/* Public type definition -------------------------------------------------- */

/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */
void Auto_InitProtections(void);
BOOL  Auto_AutoTestChecks(void);

#endif  /* AUTOTEST_H */
/*** (c) 2016 SPAL Automotive ****************** END OF FILE **************/
