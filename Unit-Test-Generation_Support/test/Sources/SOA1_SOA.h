/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  SOA1_SOA.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file SOA1_SOA.h
    \version see \ref FMW_VERSION 
    \brief Basic SOA functions prototypes.
    \details Routines for SOA operation and variables related initialization.\n
    \note none
    \warning none
*/

#ifndef SOA_H
#define SOA_H


/* Public Constants -------------------------------------------------------- */
#define SOA_LIMIT_CURRENT_PU_LOW    (u16)((float32_t)((float32_t)((float32_t)ALIGNMENT_CURRENT_AMP_REF*(float32_t)3)*(float32_t)CURRENT_RES_FOR_PU_BIT)/(float32_t)BASE_CURRENT_AMP)
#define SOA_LIMIT_CURRENT_PU_HIGH   (u16)((float32_t)((float32_t)SOA_LIMIT_CURRENT_AMP*(float32_t)CURRENT_RES_FOR_PU_BIT)/(float32_t)BASE_CURRENT_AMP)
#define SOA_LIMIT_SHIFT             (u16)8
#define SOA_LIMIT_SCALER            ((u16)1<<SOA_LIMIT_SHIFT)
#define SOA_LIMIT_CURRENT_PU_COEFF  (u16)((float32_t)((float32_t)((float32_t)SOA_LIMIT_CURRENT_PU_HIGH-(float32_t)SOA_LIMIT_CURRENT_PU_LOW)*(float32_t)SOA_LIMIT_SCALER)/(float32_t)((float32_t)EL_FREQ_NOMINAL_PU_RES_BIT-(float32_t)EL_FREQ_MIN_PU_RES_BIT))

/* Public Functions prototypes --------------------------------------------- */
void SOA_InitSOAMachine(void);
BOOL SOA_CheckSOA(void);

#endif  /* SOA_H */

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
