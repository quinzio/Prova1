/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  LOG0_LogDataS12.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file LOG0_LogDataS12.h
    \version see \ref FMW_VERSION 
    \brief Data logging into Eeprom
    \details This routines and variables manage data logging.\n
    \note none
    \warning none
*/

#ifndef LOGDATA_H
#define LOGDATA_H  

/* Public Constants -------------------------------------------------------- */

/* Suddivisione del vettore velocita' (in valori %)                     */
/* Il range 0-50 viene ricavato epr differenza col valore dell'Orologio */
#define V0    50.0 
#define V1    80.0
 
#define T0     0
#define T1    90 
#define T2   125 

/* #define FREQLOGTH   0x0100 */
#define FREQLOGTH_HZ  2 /* [Hz] */
#define FREQLOGTH     (u16)((u32)((u32)FREQLOGTH_HZ*FREQUENCY_RES_FOR_PU_BIT)/BASE_FREQUENCY_HZ)

#define DATA_OK     0x01u
#define DATA_KO     0x00u
#define CCOB_DEL    0x1200u
#define CCOB_PRO    0x1100u
#define FSTAT_START 0x80u
#define FSTAT_CLEAR 0x30u
#define FCLK_20MHZ  0x14u
#define FCLK_EN     0x00u
#define FCLK_DIS    0x80u
#define BANK1BEG    0x0540u
#define BANK2BEG    0x0580u
#ifdef _CANTATA_
#define ADDR_BANK1BEG
#define ADDR_BANK2BEG
#else
#define ADDR_BANK1BEG    @0x0540u
#define ADDR_BANK2BEG    @0x0580u
#endif
#define BANK_DIM    64u
#define ERRTYPENO   8u
#define TVECTDIM    3u
#define VVECTDIM    2u
#define VVECT0      ((u8)(V0*2.56))
#define VVECT1      ((u8)(V1*2.56))
#define TVECT0      ((u8)(T0))
#define TVECT1      ((u8)(T1))
#define TVECT2      ((u8)(T2))

#ifdef LOG_DATI
typedef enum action{
  ACT_NOACTION,
  ACT_PROGRESS,
  ACT_SAVE_PERIODIC,
  ACT_SAVE_ONLY
}action_t;
#endif

#define U16LOGDATIPERIOD_MSEC     ((u16)14065)         /* 14.065 sec        */
/*#define U8PERIODICSAVEPERIOD_SEC  ((u16)(1800)) */   /* 30 min = 1800 sec */
#define U8PERIODICSAVEPERIOD_SEC  ((u16)(900))         /* 15 min = 900 sec  */
#define LOG_DATI_TIMEOUT_BEFORE_SLEEP_MSEC ((u16)500)

#define U16LOGDATIPERIOD          ((u16)(U16LOGDATIPERIOD_MSEC/8u))  /* 1758 = 14.06 / 8ms */
#define U8PERIODICSAVEPERIOD      ((u8)(((u32)U8PERIODICSAVEPERIOD_SEC*1000u)/U16LOGDATIPERIOD_MSEC))

#define LOG_DATI_TIMEOUT_BEFORE_SLEEP_BIT ((u16)((u16)LOG_DATI_TIMEOUT_BEFORE_SLEEP_MSEC/8u))

/* Public type definition -------------------------------------------------- */

#ifdef _CANTATA_
typedef enum save_mode_e
{
  SAVE_AND_RESET,
  SAVE_ONLY
}save_mode_t;
#endif

struct ErroreLog{
  u8  Codice;
  u16 Tempo;
  u8  Tensione;
  u8  Temperatura;
  u8  Velocita;
  u8  Corrente;
};

volatile struct EeLog{
  u8  Orologio[3];
  u8  TmaxTle;
  u8  TmaxMOSFET;
  u8  Accensioni[3];
  u8  ContaErrore[ERRTYPENO];
  u8  LogT[TVECTDIM][3];
  u8  LogV[VVECTDIM][3];
  struct ErroreLog ErroreI;
  u8  Salvataggi[3];
  u8  Free1[21];
  u8  DatiIntegri;          /* NON spostare DatiIntegri da questa posizione       */
  u8  Checksum;             /* Deve essere nell'ultimo settore che viene scritto. */
};                          /* (Vedi salvataggio dati)                            */

union EeSpace{ /*PRQA S 0750 #union  for memory management*/
  struct EeLog ee;
  u8 by[BANK_DIM]; 
};

typedef union EeSpace td_EeSpace;

typedef enum
{
  /* Non alterare le posizioni degli stati. */
  LOG_START       = 0,
  LOAD_RAM        = 1,  /* non spostare LOAD_RAM da questa posizione */
  CHECK_BANK_LOOP = 2,
  CHECK_BANK      = 3,
  ERASE_SET       = 4,
  ERASE_WAIT      = 5,
  RESET_BANK_SET  = 6,
  RESET_BANK_WAIT = 7,
  BANK_CLEAR_SET  = 8,
  BANK_CLEAR_WAIT = 9,
  BAD_MEM         = 10,
  BANK_INTEGRITY  = 11,
  LOG_IDLE        = 12,
  LOG_SAVE_INIT   = 13,
  LOG_SAVE_LAUNCH = 14,
  LOG_SAVE_WAIT   = 15,
  LOG_SAVE_END    = 16,
  LOG_SAVE_IDLE   = 17
} td_LogState;

/* Public Variables -------------------------------------------------------- */
#pragma DATA_SEG SHORT _DATA_ZEROPAGE

#pragma DATA_SEG DEFAULT
#ifdef LOG_DATI
extern u16 u16LogPeakCurrentPU;
#endif

/* Public Functions prototypes --------------------------------------------- */
#ifdef LOG_DATI
  void  LOG0_LogError(u16 codice, u16 tensione, s16 tempMOSFET, u16 velocita, u16 corrente);
  void  LOG0_DoLogData(SystStatus_t State);
  void  LOG0_InitLogdati(void);
  void  LOG0_DataSaveOnly(void);
#endif
#endif
/*** (c) 2014 SPAL Automotive ****************** END OF FILE **************/
