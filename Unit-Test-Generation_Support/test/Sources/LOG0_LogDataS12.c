/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  LOG0_LogDataS12.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/**
    \file LOG0_LogDataS12.c
    \version see \ref FMW_VERSION
    \brief Read, manages and updates the DataLog structure stored in Eeprom
      memory
    \details NOTE:
      L'orologio in Ram, contiene il valore che andra' SOMMATO all'orologio
      in Eeprom. Tuttavia all'inizializzazione viene caricato in RAM il valore
      dell'orologio in Eeprom e rimarra' tale fino a che non viene sovrascritto
      la prima volta dalla chiamata UpdTimer. Se prima della chiamata, viene
      fatto il salvataggio in Eeprom dell'immagine della RAM. allora si
      potrebbero sommare i due tempi, raddoppiando il valore corretto. Valutare
      questo rischio.

*/

#ifdef _CANTATA_
#include "cantata.h"
#endif /*_CANTATA_*/

#include "SPAL_Setup.h"
#include "SpalTypes.h"
#include "SpalBaseSystem.h"
#include MICRO_REGISTERS
#include SPAL_DEF
#include MAC0_Register    /* Needed for macro definitions */
#include ADM0_ADC         /* Per la macro ADC_READING_LENGHT richiamata in SpalBaseSystem.h */
#include "main.h"
#include "LOG0_LogDataS12.h"
#include "LOG0_DataInitS12.h"
#include "WTD1_Watchdog.h"
#include "SOA1_SOA.h"
#include "MCN1_acmotor.h"
#include "TIM1_RTI.h"
#include "TMP1_Temperature.h"
#include "VBA1_Vbatt.h"   /* Per le costanti V_DROP_VDHS_PU, V_GAMMA_VDH_RVP_DIODE_PU, VBUS_HYSTERESIS_RVP_OFF_PU */
#include "RVP1_RVP.h"     /* Per la funzione RVP_IsRVPOn() */
#include <stddef.h>
#include "EMR0_Emergency.h"

#ifdef LOG_DATI

/*#define DEBUG_LOGDATA */
/*#define DEBUG_LOG_CHKUPDATE */
/* -------------------------- Private constants ---------------------------- */

static u16 LogErrors[ERRTYPENO+1u]=
{
  NO_ERROR,
  W_PROTECTION,
  START_ERROR,
  T_AMB,
  I_LIM,
  GD_ERROR,
  FAULT_ERROR,
  LOCKED_ROTOR,
  RPI_PROTECTION
};

#define EEB 4u

/* --------------------------- Public variables -----------------*/

/* --------------------------- Private variables -----------------*/
#pragma DATA_SEG DEFAULT
/**
    \var EeSpaceRAM
    \brief Working space of DataLog structure placed in RAM.\n
    \details During run time, software modifies this structure.\n
    \note The clocks in these area are initialized to zero when the
    variable is loaded in RAM.
    \warning
*/
static td_EeSpace EeSpaceRAM; /*PRQA S 0759 #union for memory management*/
/* inizializzazione generico dato nel banco */
static u16   BlkIni[2]= {0x0000u,0x0000u};
/* suddivisione estremo sinistro colonne della matrice vita */
static const u8 vec_v[VVECTDIM]= {VVECT0, VVECT1};
/* suddivisione estremo inferiore righe della matrice vita */
static const u8 vec_t[TVECTDIM]= {TVECT0, TVECT1, TVECT2};

#ifndef _CANTATA_
typedef enum
{
  SAVE_AND_RESET,
  SAVE_ONLY
}save_mode_t;
#endif /* _CANTATA_ */

static td_LogState state = LOG_START;
/* Attivare la define DEBUG_LOG_DATA per lavorare con i due banchi */
/* di memorizzazione in RAM invece che in EEPROM. In questo modo */
/* si possono modificare i valori a piacere. */
/* Nelle righe sottostanti c'e' la possibilita' di modificare i */
/* valori. */
#ifndef DEBUG_LOGDATA
#pragma DATA_SEG EEPROM_DATA
/* dichiarazione dei banchi nella zona Eeprom */
#ifdef _CANTATA_
static td_EeSpace EeSpace1;
static td_EeSpace EeSpace2;
#else
static td_EeSpace EeSpace1 ADDR_BANK1BEG;  /*PRQA S 0759 # mandatory for memory allocation */
static td_EeSpace EeSpace2 ADDR_BANK2BEG;  /*PRQA S 0759 # mandatory for memory allocation */
#endif /* _CANTATA_ */
#pragma DATA_SEG DEFAULT
#else
#pragma DATA_SEG DEFAULT
/* Attenzione: mettere 63 inizializzazioni e non 64 !!! */
/* Una delle variabili e' u16 e non u8 e quindi va scritto */
/* solo 1 valore di 2 byte per quella variabile */
/* (vedi struttura dati: e' la 33° variabile). */
td_EeSpace EeSpace1 = LOGDATA_INIT1
td_EeSpace EeSpace2 = LOGDATA_INIT2

#endif

/* static u16 u16LifeHours; */
u16 u16LogPeakCurrentPU = 0;

/* ---------------------------private prototipe-----------------*/

static void  LOG0_AddLogTimer(s16 tempMOSFET, u16 velocita, u16 velocitaMAX);
static void  LOG0_EraseEESect(const u8 *address);
static void  LOG0_ProgEESect(const u16* destadd, const u16* sourceadd);
static void  LOG0_UpdateChk(void);
static void  LOG0_UpdTTle(s16 tempe);
static void  LOG0_UpdTMOSFET(s16 tempe);
static u8    LOG0_tempplus(s16 temp);
static u32   Getu32val(const u8 *Orologio);
static void  Setu32val(u8 *Orologio, u32 val);
static int   CalcCheck(int bank);
/* u8    LOG0_isEEdone(void); */

/* ---------------------------public prototipe------------------*/

/* ---------------------------functions definition------------------*/
static td_LogState LOG0_DataLogInit(action_t action)
{
  #if defined(_CANTATA_) && !defined(_CANTATA_INTEGR_)
  #undef static
  #endif

  static u8  counter;
#ifdef DEBUG_LOGDATA
#ifdef DEBUG_LOG_CHKUPDATE
  static u8  sum = 0u;
#endif
#endif
  static td_EeSpace  *ps;
  static td_EeSpace  *pd;
  static u8 i, j;
  static u32 val;
  static int  bank;
  static save_mode_t save_mode;
  int ret_CalcCheck;
  int ret_CalcCheck_1u;
  int ret_CalcCheck_2u;
  #if defined(_CANTATA_) && !defined(_CANTATA_INTEGR_)
  #define static
  #endif

  switch (state)
  {
  case LOG_START:

#ifdef DEBUG_LOGDATA
#ifdef DEBUG_LOG_CHKUPDATE
    sum = 0u;
    for (i=0u; i< BANK_DIM -1u; i++) sum += EeSpace1.by[i];
    EeSpace1.by[i] = (sum ^ 0xFFu)+1u;
    sum = 0u;
    for (i=0u; i< BANK_DIM -1u; i++) sum += EeSpace2.by[i];
    EeSpace2.by[i] = (sum ^ 0xFFu)+1u;
#endif
#endif

    state = LOAD_RAM;
    break;

  case CHECK_BANK_LOOP:
    bank++;
    if (bank == (int)3) 
    {
      state = BANK_INTEGRITY;
    }
    else
    {
      if (bank == (int)1) 
      {
        pd = &EeSpace1;
      }
      if (bank == (int)2)
      {
        pd = &EeSpace2;
      }
      counter = 0u;
      state = CHECK_BANK;
    }
    break;

  case CHECK_BANK:

    ret_CalcCheck = CalcCheck(bank);

    if ((Getu32val(pd->ee.Orologio) == 0x00FFFFFFu) &&
        (ret_CalcCheck != (int)0) &&
        ((pd->ee.DatiIntegri) != 0u))
    {
      state = ERASE_SET;
    }
    else
    {
      state = CHECK_BANK_LOOP;
    }
    break;

  case ERASE_SET:
    if (counter >= BANK_DIM)
    {
      counter = 0u;
      state = RESET_BANK_SET;
    }
    else
    {
      LOG0_EraseEESect((const u8*)pd + counter);  /*PRQA S 0488 #Mandatory for memory management */
      state = ERASE_WAIT;
    }
    break;

  case ERASE_WAIT:
    if (FSTAT_CCIF == 1u)
    {
      counter += 4u;
      state = ERASE_SET;
    }
    break;

  case RESET_BANK_SET:
    if (counter >= BANK_DIM)
    {
      state = CHECK_BANK_LOOP;
    }
    else
    {
      LOG0_ProgEESect((const u16*)((u8 *)pd + counter), BlkIni);  /* PRQA S 0488,0310,3305 # mandatory to access to EEPROM structure*/
      state = RESET_BANK_WAIT;
    }
    break;

  case RESET_BANK_WAIT:
    if (FSTAT_CCIF == 1u)
    {
      counter += 4u;
      state = RESET_BANK_SET;
    }
    break;

  case BANK_INTEGRITY:
    i = 1u;
    j = 1u;/* entrambi i banchi integri */
    if (EeSpace1.ee.DatiIntegri != 0u) 
    {
      i=0u;
    }
    if (EeSpace2.ee.DatiIntegri != 0u)
    {
      j=0u;
    }
    if (CalcCheck((int)1) != (int)0)
    {
      i=0u;
    }
    if (CalcCheck((int)2) != (int)0)
    {
      j=0u;
    }
    if ((i==0u) && (j==0u))
    {
      state = BAD_MEM;
    }
    if ((i==1u) && (j==0u))
    {
      counter = 0u;
      pd = &EeSpace2;
      state = BANK_CLEAR_SET;
    }
    if ((i==0u) && (j==1u))
    {
      counter = 0u;
      pd =  &EeSpace1;
      state = BANK_CLEAR_SET;
    }
    if ((i==1u) && (j==1u))
    {
      if (Getu32val(EeSpace1.ee.Salvataggi) <= Getu32val(EeSpace2.ee.Salvataggi))
      {
        pd = &EeSpace1;
      }
      else
      {
        pd = &EeSpace2;
      }
      counter = 0u;
      state = BANK_CLEAR_SET;
    }
    break;

  case BANK_CLEAR_SET:
    if (counter >= BANK_DIM)
    {
      state = LOG_IDLE;
    }
    else
    {
      LOG0_EraseEESect((const u8*)pd + counter); /*PRQA S 0488 #Mandatory for memory management */
      state = BANK_CLEAR_WAIT;
    }
    break;

  case BANK_CLEAR_WAIT:
    if (FSTAT_CCIF == 1u)
    {
      counter += 4u;
      state = BANK_CLEAR_SET;
    }
    break;

  case LOAD_RAM:
    i = 1u;
    j = 1u;
    ret_CalcCheck_1u = CalcCheck((int)1);
    ret_CalcCheck_2u = CalcCheck((int)2);

    if ((Getu32val(EeSpace1.ee.Orologio) == 0x00FFFFFFu) &&
        (ret_CalcCheck_1u !=(int) 0) &&
        ((EeSpace1.ee.DatiIntegri) != 0u))
    {
      i=0u;
    }

    if ((Getu32val(EeSpace2.ee.Orologio) == 0x00FFFFFFu) &&
        (ret_CalcCheck_2u != (int)0) &&
        ((EeSpace2.ee.DatiIntegri) != 0u))
    {
      j=0u;
    }
    if (EeSpace1.ee.DatiIntegri != 0u)
    {
      i=0u;
    }
    if (EeSpace2.ee.DatiIntegri != 0u)
    {
      j=0u;
    }
    if (CalcCheck((int)1) != (int)0)
    {
      i=0u;
    }
    if (CalcCheck((int)2) != (int)0)
    {
      j=0u;
    }

    if ((i==0u) && (j==0u))
    {
      ps = NULL;
    }
    if ((i==1u) && (j==0u))
    {
      ps = &EeSpace1;  /* i = 1: ps punta a banco 1 ok */
    }
    if ((i==0u) && (j==1u))
    {
      ps =  &EeSpace2; /* j = 1: ps punta a banco 2 ok */
    }
    if ((i==1u) && (j==1u))  /*Entrambi buoni --> Scelgo quello con l'ora piu' recente */
    {
      if (Getu32val(EeSpace1.ee.Salvataggi) <= Getu32val(EeSpace2.ee.Salvataggi))
      {
        ps = &EeSpace2;
      }
      else
      {
        ps = &EeSpace1;
      }
    }

    if (ps != NULL)   /*Carico il banco buono in RAM */
    {
      for (counter=0u; counter<BANK_DIM; counter++)
      {
        *((u8 *)&EeSpaceRAM + counter) = *((u8 *)ps + counter); /*PRQA S 0488 #Mandatory for memory management */
      }
      /* Incrementa contatore power-on */
      val = Getu32val(EeSpaceRAM.ee.Salvataggi) + 1u;
      if ((val & 0x00FFFFFFu) != 0u)
      {
        Setu32val(EeSpaceRAM.ee.Salvataggi, val);
      }
    }
    else
    { /*Verifico se i due banchi sono vergini (0xFF) */
      i=1u;
      for(counter = 0u; counter < BANK_DIM; counter++)
      {
        if ( (EeSpace1.by[counter] != 0xFFu) || (EeSpace2.by[counter] != 0xFFu) )
        {
          i=0u; 
          break;
        }
      }
      if (i==1u)
      {
        /* I due banchi sono tutti a 0xFF, imposta a 1 il numero di Salvataggi. */
        Setu32val(EeSpaceRAM.ee.Salvataggi, (u32)1);
      }
    }
    bank =(int)0;
    state = CHECK_BANK_LOOP;
    break;

  case LOG_IDLE:
    /* pd punta al banco di riferimento */
    if ((action == ACT_SAVE_ONLY) || (action == ACT_SAVE_PERIODIC))
    {
      state = LOG_SAVE_INIT;
    }
    if (action == ACT_SAVE_ONLY)
    {
      save_mode = SAVE_ONLY;
    }
    if (action == ACT_SAVE_PERIODIC)
    {
      save_mode = SAVE_AND_RESET;
    }
    break;

  case LOG_SAVE_INIT:
  {
    /* pd = &EeSpace?; pd e' gia' impostato */
    ps = &EeSpaceRAM;
    EeSpaceRAM.ee.DatiIntegri = 0u;
    LOG0_UpdateChk();
    state = LOG_SAVE_LAUNCH;
    counter = 0u;
    break;
  }

  case LOG_SAVE_LAUNCH:
  {
    j = 1u;
    for (i=0u; i<EEB; i++) 
    {       
      if (*((u8*)pd+counter+i) != *((u8*)ps+counter+i)) /*PRQA S 0488 #Mandatory for memory management */
      {
        j = 0u;
        break;
      }
    }
    if (j==0u)
    {
      LOG0_ProgEESect((const u16*)((u8 *)pd + counter), (const u16*)((u8 *)ps+counter));  /* PRQA S 0310,0488,3305 # mandatory to access to EEPROM structure*/
      state = LOG_SAVE_WAIT;
    }
    else
    {
      state = LOG_SAVE_END;
    }
    break;
  }

  case LOG_SAVE_WAIT:
  {
    if(FSTAT_CCIF == 1u)
    {
      state = LOG_SAVE_END;
    }
    break;
  }

  case LOG_SAVE_END:
  {
    counter += EEB;
    if (counter < BANK_DIM)
    {
      state = LOG_SAVE_LAUNCH;
    }
    else
    {
      if(save_mode == SAVE_AND_RESET) 
      {
        state = LOG_START;
      }
      if(save_mode == SAVE_ONLY) 
      {
        state = LOG_SAVE_IDLE;
      }
    }
    break;
  }

  case LOG_SAVE_IDLE:
    /* stato senza uscita, richiede reset microprocessore */
    /* per ripartire */
    break;
  default:
    EMR_EmergencyDisablePowerStage();
    RESET_MICRO;
    break;
  }
  return state;
}



static void LOG0_AddLogTimer(s16 tempMOSFET, u16 velocita, u16 velocitaMAX)
{
  u8 i;
  u32 val;
  s16 tempMOSFETConv;

  tempMOSFETConv = (s16)(((s16)tempMOSFET*MOS_BASE_TEMPERATURE_DEG)/(s16)((u32)TEMPERATURE_RES_FOR_PU_BIT));

  if (strMachine.u16FrequencyAppPU > FREQLOGTH)
  {
    val = Getu32val(EeSpaceRAM.ee.Orologio) + 1u;
    if ((val & 0x00FFFFFFu) != 0u)
    {
      Setu32val(EeSpaceRAM.ee.Orologio,  val);
    }
    /* Determinazione degli intervalli di velocita' */
   /* PRQA S 1338 ++ # No side effect expected*/ 
    velocita = (u16)((((u32)velocita)<<8)/velocitaMAX);   /* 255: velocita' max */
   /*PRQA S 1338 --*/ 
    /* se velocita<vec_v[0] incrementa nulla */
    /* se vec_v[0]<velocita<vec_v[1] incrementa LogV[0] */
    /* se vec_v[1]<velocita<vec_v[2] incrementa LogV[1] */
    /* ecc... */
    for (i=0u; i < VVECTDIM ; i++)
    {
      if (vec_v[i] > velocita)
      {
        break;
      }
    }
    if (i != 0u)
    {
      i--;
      val = Getu32val(EeSpaceRAM.ee.LogV[i]) + 1u;
      if ((val & 0x00FFFFFFu) != 0u)
      {
        Setu32val(EeSpaceRAM.ee.LogV[i], val);
      }
    }

    for (i=0u; i < TVECTDIM ; i++)
    {
      if ((s16)vec_t[i] > tempMOSFETConv)
      {
        break;
      }
    }
    if (i != 0u)
    {
      i--;
      val = Getu32val(EeSpaceRAM.ee.LogT[i]) + 1u;
      if ((val & 0x00FFFFFFu) != 0u)
      {
        Setu32val(EeSpaceRAM.ee.LogT[i], val);
      }
    }
  }
}



void LOG0_LogError(u16 codice, u16 tensione, \
                   s16 tempMOSFET, u16 velocita, u16 corrente)
{

  u8  id;
  u8  tempPlusMOS;
  u32 tempo;

  /* Filtraggio codici di errore ammessi */
  for (id=0u; id<ERRTYPENO; id++)
  {
    if (codice == LogErrors[id]) 
    {
      break;
    }
  }
  if (id != ERRTYPENO)
  {
    tempPlusMOS = LOG0_tempplus((s16)((s32)((s32)tempMOSFET*TLE_BASE_TEMPERATURE_DEG)/(s32)TEMPERATURE_RES_FOR_PU_BIT));
    tempo = Getu32val(EeSpaceRAM.ee.Orologio);
    tempo >>= 8;
    /* nel campo codice l'msb serve per segnalare che i campi */
    /* sono scritti (l'orologio puo' essere a zero) */
    if (EeSpaceRAM.ee.ErroreI.Codice == NO_ERROR)
    {
      EeSpaceRAM.ee.ErroreI.Codice = id;
      EeSpaceRAM.ee.ErroreI.Tempo = (u16)tempo;

    #ifdef RVP_SW_MANAGEMENT_ENABLE
      if ( (BOOL)RVP_IsRVPOn() == (BOOL)FALSE )
      {
        EeSpaceRAM.ee.ErroreI.Tensione = (u8)((u16)((u16)((u16)tensione-(u16)V_DROP_VDHS_PU-(u16)V_GAMMA_VDH_RVP_DIODE_PU-(u16)VBUS_HYSTERESIS_RVP_OFF_PU)/BASE_VOLTAGE_CONV_FACT_RES)>>2);
      }
      else
      {
        EeSpaceRAM.ee.ErroreI.Tensione = (u8)((u16)((u16)((u16)tensione-(u16)V_DROP_VDHS_PU-(u16)V_GAMMA_VDH_RVP_DIODE_PU)/BASE_VOLTAGE_CONV_FACT_RES)>>2);
      }
    #endif  /* RVP_SW_MANAGEMENT_ENABLE */

      EeSpaceRAM.ee.ErroreI.Temperatura = tempPlusMOS;
      EeSpaceRAM.ee.ErroreI.Velocita = (u8)((u16)((u32)((u32)velocita*BASE_FREQUENCY_HZ)/FREQUENCY_RES_FOR_PU_BIT)>>1);
      EeSpaceRAM.ee.ErroreI.Corrente = (u8)((u16)((u16)corrente/BASE_CURRENT_CONV_FACT_RES)>>2);
    }
    if (EeSpaceRAM.ee.ContaErrore[id-(u8)1] != 0xFFu)
    {
      EeSpaceRAM.ee.ContaErrore[id-(u8)1]++;
    }
  }
  else
  {
    /*return;*/
     /* il codice di errore non e' da registrare */
  }
}

static void LOG0_EraseEESect(const u8 *address)
{
#ifdef DEBUG_LOGDATA
  int i;
  for (i=0; i<4; i++) *(address++) = 0xFF;
#else
   FCLKDIV = FCLK_EN;              /*0x00 abilita scrittura FCLKDIV */
   FCLKDIV = FCLK_20MHZ;           /*0x14 scrivi registro     "     Fbus = 20 MHz see MC9S12G Datasheet table 21-8 */
   FCLKDIV = FCLK_DIS|FCLK_20MHZ;  /*0x54 blocca scrittura    " */
   FSTAT = FSTAT_CLEAR;
   FCCOBIX = 0u;
   FCCOB = CCOB_DEL;
   FCCOBIX = 1u;
   FCCOB = (u16)address;  /*PRQA S 0306 #Mandatory for memory management*/
   FSTAT |= FSTAT_START; /* Lancia comando */
#endif
}

static void  LOG0_ProgEESect(const u16* destadd, const u16* sourceadd)
{
#ifdef DEBUG_LOGDATA
  int i;
  for (i=0; i<2; i++) *(destadd++) = *(sourceadd++);
#else
   FCLKDIV = FCLK_EN;              /*0x00 abilita scrittura FCLKDIV */
   FCLKDIV = FCLK_20MHZ;           /*0x14 scrivi registro     "     Fbus = 20 MHz see MC9S12G Datasheet table 21-8 */
   FCLKDIV = FCLK_DIS|FCLK_20MHZ;  /*0x54 blocca scrittura    " */
   FSTAT = FSTAT_CLEAR;
   FCCOBIX = 0u;
   FCCOB = CCOB_PRO;
   FCCOBIX = 1u;
   FCCOB = ((u16)destadd) & (u16)0xFFFCu;  /* deve essere multiplo di 4 */  /*PRQA S 0306  #cast mandatory*/ 
   FCCOBIX = 2u;
   FCCOB = *sourceadd;
   sourceadd++;   /* PRQA S 1338 # No side effect expected*/
   FCCOBIX = 3u;
   FCCOB = *sourceadd;
   FSTAT |= FSTAT_START; /* Lancia comando */
#endif
}

static void LOG0_UpdateChk(void)
{
  u16 count;
  u8  csum=0;
  /* Il checksum finale non va contato ! */
  for (count=0u; count<(BANK_DIM-1u); count++)
  {
    csum += EeSpaceRAM.by[count];
  }
  EeSpaceRAM.ee.Checksum  = (u8)((~csum)+1u);
}

static void LOG0_UpdTTle(s16 tempe)
{
  u8 t_plus;
  t_plus = LOG0_tempplus((s16)((s32)((s32)tempe*TLE_BASE_TEMPERATURE_DEG)/(s32)TEMPERATURE_RES_FOR_PU_BIT));
  
  if (EeSpaceRAM.ee.TmaxTle < t_plus)
  {
    EeSpaceRAM.ee.TmaxTle = t_plus;
  }
}

static void LOG0_UpdTMOSFET(s16 tempe)
{
  u8 t_plus;
  t_plus = LOG0_tempplus((s16)((s32)((s32)tempe*MOS_BASE_TEMPERATURE_DEG)/(s32)TEMPERATURE_RES_FOR_PU_BIT));

  if (EeSpaceRAM.ee.TmaxMOSFET < t_plus)
  {
    EeSpaceRAM.ee.TmaxMOSFET = t_plus;
  }
}

static u8 LOG0_tempplus(s16 temp)
{
  u8 ret_tmp;
  /* Conversione temperature... */
  /* se temp<=0 -> 0 */
  /* se temp>0  -> invariato */
  if (temp<=0)
  {
    ret_tmp = 0;
  }
  else
  {
    ret_tmp = (u8)temp;
  }

  return (ret_tmp);
}

static u32 Getu32val(const u8 *Orologio)
{
  u32 x=0;
  s8  i=2;
  while (i>=0)
  {
    x = (x<<8) + Orologio[i];
    i--;
  }
  return x;
}

static void Setu32val(u8 *Orologio, u32 val)
{
  s8  i;
  for (i=0; i<3; i++)
  {
    Orologio[i] = (u8)(val & 0xFFu);
    val >>= 8; /* PRQA S 1338 # No side effect expected*/
  }
}

static int CalcCheck(int bank)  /*PRQA S 2889 #Mandatory for error management*/
{
  u8 i;
  u8 sum=0;
  int ret;
  volatile u8 *p;
  if (bank == (int)1)
  {
    p = (volatile u8*)&EeSpace1;
  }
  else if (bank == (int)2)
  {
    p = (volatile u8*)&EeSpace2;
  }
  else
  {
    return ((int)0);
  }
  /* L'ultima locazione e' il checksum */
  for (i=0; i<BANK_DIM; i++)
  {
    sum += *(p + i); /*PRQA S 0488 #Mandatory for memory management */
  }
  if(sum > 0u)
  {
    ret = (int)1;
  }
  else
  {
    ret = (int)0;
  }
  return (ret);
}

void LOG0_DoLogData(SystStatus_t State)
{
  #ifdef LOG_PERIODICSAVE_ENABLE

  #if defined(_CANTATA_) && !defined(_CANTATA_INTEGR_)
  #undef static
  #endif

  static u8 Log_periodicsave = 0u;

  #if defined(_CANTATA_) && !defined(_CANTATA_INTEGR_)
  #define static
  #endif

  #endif /*LOG_PERIODICSAVE_ENABLE */
  if (State == RUN)
  {
    if (LOG0_DataLogInit(ACT_NOACTION) == LOG_IDLE)
    {
      /* not busy */
      if (RTI_IsLogDatiElapsed() == (BOOL)TRUE)
      {
        #ifdef MOSFET_OVERTEMPERATURE_PROTECTION
          LOG0_AddLogTimer(Temp_GetTMos(), strMachine.u16FrequencyAppPU, EL_FREQ_MAX_PU_RES_BIT);
        #else
          LOG0_AddLogTimer(Temp_GetTamb(), strMachine.u16FrequencyAppPU, EL_FREQ_MAX_PU_RES_BIT);
        #endif  // MOSFET_OVERTEMPERATURE_PROTECTION

        LOG0_UpdTTle(Temp_GetTamb());

        #ifdef MOSFET_OVERTEMPERATURE_PROTECTION
          LOG0_UpdTMOSFET(Temp_GetTMos());
        #else
          LOG0_UpdTMOSFET(Temp_GetTamb());
        #endif  /* MOSFET_OVERTEMPERATURE_PROTECTION */

        #ifdef LOG_PERIODICSAVE_ENABLE
        Log_periodicsave++;
        if (Log_periodicsave >= U8PERIODICSAVEPERIOD)  
        {
          (void)LOG0_DataLogInit(ACT_SAVE_PERIODIC);
          Log_periodicsave = 0u;
        }
        #endif /*LOG_PERIODICSAVE_ENABLE */
      }
    }
  }
  else
  {
    RTI_SetDatiLogPeriod(U16LOGDATIPERIOD);
  }
  (void)LOG0_DataLogInit(ACT_PROGRESS);
}

void  LOG0_InitLogdati(void)
{
  u8 counter;
  u32 val;

  PROTECT_EEPROM_TRIMMING_PARAMETERS;

  for(counter = 0u; counter <BANK_DIM; counter++)
  {
    EeSpaceRAM.by[counter]=0x00u;
  }
  RTI_SetDatiLogPeriod(U16LOGDATIPERIOD);
  while (LOG0_DataLogInit(ACT_PROGRESS) <= LOAD_RAM){}
  /* Questa routine viene chiamata solo al power-on, e quindi solo */
  /* in questo caso viene incrementato il numero di accensioni. */
  val = Getu32val(EeSpaceRAM.ee.Accensioni) + 1u;
  if ((val & 0x00FFFFFFu) != 0u)
  {
    Setu32val(EeSpaceRAM.ee.Accensioni, val);
  }
}


void LOG0_DataSaveOnly(void)
{
  BOOL ret;
  (void)LOG0_DataLogInit(ACT_SAVE_ONLY);
  RTI_SetDatiLogPeriod(LOG_DATI_TIMEOUT_BEFORE_SLEEP_BIT);
  ret = RTI_IsLogDatiElapsed();
  while((LOG0_DataLogInit(ACT_SAVE_ONLY)!= LOG_SAVE_IDLE) && (ret==(BOOL)FALSE))
  {
    WTD_ResetWatchdog();
  }
}

#endif //LOG_DATI
