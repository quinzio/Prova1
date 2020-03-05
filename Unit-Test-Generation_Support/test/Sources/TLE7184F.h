/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  TLE7184F.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file TLE7184F.h
    \version see \ref FMW_VERSION 
    \brief Basic TLE7184F functions prototypes.
    \details Routines for basic TLE7184F operation and variables related initialization.\n
    \note none
    \warning none
*/

#ifndef TLE7184F_H
#define TLE7184F_H


/* --------------------------- Public Constants ------------------------------*/
#define TLE_CURR_ERROR_DETECT_LEVEL_AMP   3        /* If current is less than this value -> TLE in error! */
#define TLE_CURR_ERROR_DETECT_LEVEL_PU    (u16)((u32)((u32)TLE_CURR_ERROR_DETECT_LEVEL_AMP*CURRENT_RES_FOR_PU_BIT)/(u32)BASE_CURRENT_AMP)
#define TLE_ERROR_COUNT                   (u8)20   /* This value has to be kept less the minimum motor current. */
                                                   /* If TLE error line is off and consegutive convertion current are less than TLE_CURRENT_ERROR_DETECTION_LEVEL, */
                                                   /* after TLE_ERROR_COUNT items -> GD error! */

/* Public type definition -------------------------------------------------- */

/* Public Variables -------------------------------------------------------- */
 
/* Public Functions prototypes --------------------------------------------- */
void  TLE7184F_ResetTLEAndForceHiZ(void);
void  TLE_EnableOutputs(void);
/* void  TLE_DisableOutputs(void); */
BOOL  TLE_CheckError(void);

#endif  /* TLE7184F_H */

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
