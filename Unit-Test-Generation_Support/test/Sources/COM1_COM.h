/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  COM1_COM.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file COM1_COM.h
    \version see \ref FMW_VERSION 
    \brief HL communication protocol function prototypes.
    \details Here are stored all the prototypes referred to SPAL communication protocol. These are all HL routines and are hw/device uncorrelated.\n
    \note none
    \warning none
*/

#ifndef COM_H
#define COM_H


/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */
void Com_SendVerificationBytes(void);
void Com_SendParameterBytes(void);

#ifdef ENABLE_RS232_DEBUG
  void Com_SendSystemDataRS232(void);
  void Com_SendResetStatus(void);
  void Com_SendSystemErrRS232(void);
  void Com_AnswerRequest(void); 
  void Com_ServeRequest(void); 
  void Com_SendAutotestError(void);
  void Com_SendSystemStatus(void);
#endif /* ENABLE_RS232_DEBUG */

#endif  // COM_H

/*** (c) 2007 SPAL Automotive ****************** END OF FILE **************/
