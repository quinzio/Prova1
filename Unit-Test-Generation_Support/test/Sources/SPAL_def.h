/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  SPAL_def.h

VERSION  :  1.2

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

#ifndef SPALDEF_H
#define SPALDEF_H

/**
    \file SPAL_def.h
    \version see \ref FMW_VERSION
    \brief Header file with all the application constants.
    \details none
    \note none
    \warning none
*/
#include "stdtypes.h"
#include "SpalTypes.h"



/* Parametri motore + carico */
  #define POLE_PAIR_NUM                       (u8)6
  #define Rs_REF                              (float32_t)0.00595     /* [Ohm] */
  #define R_MOS_REF                           (float32_t)0.002       /* [Ohm] */
  #define Ls_REF                              (float32_t)0.0000356   /* [H]   */
  #define E_REF_1000RPM                       (float32_t)2.91        /* [V]   */
  #define ALIGNMENT_CURRENT_AMP_REF           (float32_t)30          /* [A]   */
  #define FAN_ROT_INERTIA                     (float32_t)0.030       /* [kg * m^2] */

/* Parametri hardware per lettura tensione e corrente */
  /* Vcc di riferimento (alimentazione micro,TLE,Vmon) */
  #define BASE_VCC_VOLT                       (float32_t)5           /* [V] */

  /* Circuito lettura corrente */
  #define SHUNT_RESISTOR                      (float32_t)0.0003      /* [Ohm] */
  #define OP_AMP_GAIN                         (float32_t)57.6
  #define BASE_OP_AMP_OFFSET                  (float32_t)0.00694     /* [V] */

  /* Circuito lettura tensione */
  #define V_MON_RESISTOR_DOWN                 (float32_t)7500        /* [Ohm] */
  #define V_MON_RESISTOR_UP                   (float32_t)47000       /* [Ohm] */
  #define V_MON_GAIN                          (float32_t)((float32_t)V_MON_RESISTOR_DOWN/(float32_t)((float32_t)V_MON_RESISTOR_UP+V_MON_RESISTOR_DOWN))

  #ifndef VDH_RVP_DIODE 
    #define V_GAMMA_VDH_RVP_DIODE             (float32_t)0           /* [V] */
  #else
    #define V_GAMMA_VDH_RVP_DIODE             (float32_t)0.4         /* [V] */
  #endif /* VDH_RVP_DIODE */

  #define V_DROP_VDHS_VOLT                    (float32_t)0.15        /* [V] */

/* Parcheggio: tempistiche e sweep di angolo */
  #define PRE_ALIGNMENT_BRAKING_TIME_SEC      (float32_t)1.5         /* [s]   */
  #define PARKING_TIME_25DEG_TEMPERATURE      (float32_t)25          /* [°C]  */
  #define PARKING_TIME_160DEG_TEMPERATURE     (float32_t)160         /* [°C]  */
  #define START_SWEEP_ANGLE_DEG               (float32_t)0           /* [°el] */
  #define TARGET_SWEEP_ANGLE_DEG              (float32_t)359         /* [°el] */
  /*#define SWEEP_TRIGGER                     (u8)1  */              /* Sweep angolo ogni 8 ms */

/* Setpoint di velocita' */
  #define RPM_MIN                             (u16)800                    /* [rpm] */
  #define RPM_NOMINAL                         (u16)3000                   /* [rpm] */
  #define RPM_MAX_SPEED                       (u16)3000                   /* [rpm] */
  #define RPM_TO_ROF                          (u16)400                    /* [rpm] */
  #define MAX_RPM_TO_BRAKE	                  (u16)1200                   /* [rpm] */
  #define VoF_INIT_FREQ_RPM	                  0   
  #define VoF_END_FREQ_RPM                    RPM_NOMINAL
  #define RPM_TO_CHANGE_MOD                   (u16)((float32_t)VoF_END_FREQ_RPM/1.6)

#ifdef REDUCED_SPEED_BRAKE_VS_TEMPERATURE
  #define RPM_BRAKE_MIN                       (u16)500           /* [rpm] */
  #define RVS_TEMPERATURE_TRESHOLD            (s16)15            /* [°C] */
#endif /* REDUCED_SPEED_BRAKE_VS_TEMPERATURE */

/* Overcurrent RoF  */
#if ( ( FAN_TYPE == BLOWER ) || defined(OVERCURRENT_ROF) )  /* Per il RoF "semplice" del blower oppure per la procedura di Overcurrent-RoF. */
  
  #define IPK_MAX_DELTA                       2.5   /* [A] */
  #define IPK_MAX_DELTA_PU                    (u16)((float32_t)((float32_t)IPK_MAX_DELTA*CURRENT_RES_FOR_PU_BIT)/BASE_CURRENT_AMP)   
  #define IPK_MAX_OVERCURRENT_NUM             10    /* 10 volte consecutive (a step di campionamento fisso). */

  #define IPK_MAX_OVERCURRENT_LIMIT           28    /* [A] (Circa il valore della SOA di spegnimento per corrente Ipk. Non ha senso un valore piu' alto, */
                                                    /* altrimenti si rischia uno spegnimento ed una ri-partenza da fermo, piuttosto che un RoF), */
  #define IPK_MAX_OVERCURRENT_LIMIT_PU        (u16)((float32_t)((float32_t)IPK_MAX_OVERCURRENT_LIMIT*CURRENT_RES_FOR_PU_BIT)/BASE_CURRENT_AMP)

#endif  /* ( FAN_TYPE == BLOWER ) || defined(OVERCURRENT_ROF) */

/* Rampe di frequenza */
  /* Tempi di accelerazione/decelerazione */
  #define FREQ_RAMP_TIME                      (u8)22           /* [s] */

/* Regolatori PI: dimensionamento */  
  /* Regolatore start */
  #define START_REG_COMPUTATION_RES_SHIFT     (u8)12  
  #define START_REG_BANDWIDTH_HZ              (float32_t)150   /* [Hz] */
  #define START_REG_COMP_RES_SHIFT_OFF        (u8)12
  #define START_REG_BANDWIDTH_HZ_OFF          (float32_t)20    /* [Hz] */
  #define MIN_PEAK_CURR                       (float32_t)13    /* [A] */

  /* Regolatore VoF */
  #define VoF_REG_COMPUTATION_RES_SHIFT       (u8)15
  #define VoF_REG_BANDWIDTH_HZ                (float32_t)1     /* [Hz] */

  #ifdef WM_MANAGEMENT
    /* Regolatore Wind-mill */
    #define WM_REG_COMPUTATION_RES_SHIFT      (u8)15
    #define MIN_PEAK_CURR_WM                  MIN_PEAK_CURR    /* [A] */
    #define WM_PHI_ANGLE_LIM_DEG              (u16)75          /* [°el] */
    #define WM_REG_BANDWIDTH_HZ               (float32_t)0.5   /* [Hz] */
  #endif  /* WM_MANAGEMENT */

  /* Regolatore Delta max */
  #define DELTA_REG_COMPUTATION_RES_SHIFT     (u8)15

  /* Regolatore Ipk max */
  #define MAX_CURR_REG_COMPUTATION_RES_SHIFT  (u8)15  
  /*#define LIM_CURRENT_AMPERE                (float32_t)120*/ /* [A] */

  /* Regolatore Ibatt max */
  /*#define IBATT_REG_COMPUTATION_RES_SHIFT   (u8)15 */

  /* Regolatore TOD */
  #define RPM_NOMINAL_TOD                     200                /* [rpm] */
  #define TOD_REG_COMPUTATION_RES_SHIFT       ((u8)10)
  #define EFFICIENCY_AT_LOWFREQ_LIMIT         ((float32_t)0.3)

  /* Regolatore Pmax */
  #define PMAX_REG_COMPUTATION_RES_SHIFT      (u8)15
  #define PMAX_REG_SETPOINT                   (u16)960          /* [W] */
  #define EFFICIENCY_AT_HIGHFREQ_LIMIT        (float32_t)0.9

  /* Regolatore di temperatura */
  #define TEMP_REG_COMPUTATION_RES_SHIFT      (u8)16
  #define TEMP_REG_BASE_STEP_TIME             (u8)60                                 /* [s] */
  #define THERMAL_RC_CONSTANT                 (u16)600                               /* [s] */
  #define BASE_TEMP_BANDWITH                  (float32_t)((float32_t)BASE_FREQUENCY_HZ/(float32_t)100)  /* [Hz] */
  #define TEMP_REG_BANDWITH                   (float32_t)0.002                       /* [Hz] */

  /* Rotore bloccato */
  #define SOA_LIMIT_CURRENT_AMP               (float32_t)130     /* [A] */
  #define SOA_STUCK_OVERCOMING_MAX            (u8)8
  
  
#endif /* SPALDEF_H */

/*** (c) 2016 SPAL Automotive ****************** END OF FILE **************/
