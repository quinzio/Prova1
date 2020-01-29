/* Based on CPU DB MC9S12GN32_48, version 3.00.002 (RegistersPrg V2.32) */
/* DataSheet : MC9S12GRMV1 Rev. 0.29 April 20, 2010 */

#include <mc9s12gn32.h>

/*lint -save -esym(765, *) */


/* * * * *  8-BIT REGISTERS  * * * * * * * * * * * * * * * */
volatile PORTESTR _PORTE;                                  /* Port E Data Register; 0x00000008 */
volatile DDRESTR _DDRE;                                    /* Port E Data Direction Register; 0x00000009 */
volatile MODESTR _MODE;                                    /* Mode Register; 0x0000000B */
volatile PUCRSTR _PUCR;                                    /* Pull-Up Control Register; 0x0000000C */
volatile DIRECTSTR _DIRECT;                                /* Direct Page Register; 0x00000011 */
volatile MMCCTL1STR _MMCCTL1;                              /* MMC Control Register; 0x00000013 */
volatile PPAGESTR _PPAGE;                                  /* Program Page Index Register; 0x00000015 */
volatile ECLKCTLSTR _ECLKCTL;                              /* ECLK Control Register; 0x0000001C */
volatile IRQCRSTR _IRQCR;                                  /* Interrupt Control Register; 0x0000001E */
volatile DBGC1STR _DBGC1;                                  /* Debug Control Register 1; 0x00000020 */
volatile DBGSRSTR _DBGSR;                                  /* Debug Status Register; 0x00000021 */
volatile DBGTCRSTR _DBGTCR;                                /* Debug Trace Control Register; 0x00000022 */
volatile DBGC2STR _DBGC2;                                  /* Debug Control Register 2; 0x00000023 */
volatile DBGCNTSTR _DBGCNT;                                /* Debug Count Register; 0x00000026 */
volatile DBGSCRXSTR _DBGSCRX;                              /* Debug State Control Register; 0x00000027 */
volatile DBGXCTLSTR _DBGXCTL;                              /* Debug Comparator Control Register; 0x00000028 */
volatile DBGXAHSTR _DBGXAH;                                /* Debug Comparator Address High Register; 0x00000029 */
volatile DBGXAMSTR _DBGXAM;                                /* Debug Comparator Address Mid Register; 0x0000002A */
volatile DBGXALSTR _DBGXAL;                                /* Debug Comparator Address Low Register; 0x0000002B */
volatile DBGADHSTR _DBGADH;                                /* Debug Comparator Data High Register; 0x0000002C */
volatile DBGADLSTR _DBGADL;                                /* Debug Comparator Data Low Register; 0x0000002D */
volatile DBGADHMSTR _DBGADHM;                              /* Debug Comparator Data High Mask Register; 0x0000002E */
volatile DBGADLMSTR _DBGADLM;                              /* Debug Comparator Data Low Mask Register; 0x0000002F */
volatile CPMUSYNRSTR _CPMUSYNR;                            /* S12CPMU Synthesizer Register; 0x00000034 */
volatile CPMUREFDIVSTR _CPMUREFDIV;                        /* S12CPMU Reference Divider Register; 0x00000035 */
volatile CPMUPOSTDIVSTR _CPMUPOSTDIV;                      /* S12CPMU Post Divider Register; 0x00000036 */
volatile CPMUFLGSTR _CPMUFLG;                              /* S12CPMU Flags Register; 0x00000037 */
volatile CPMUINTSTR _CPMUINT;                              /* S12CPMU Interrupt Enable Register; 0x00000038 */
volatile CPMUCLKSSTR _CPMUCLKS;                            /* S12CPMU Clock Select Register; 0x00000039 */
volatile CPMUPLLSTR _CPMUPLL;                              /* S12CPMU PLL Control Register; 0x0000003A */
volatile CPMURTISTR _CPMURTI;                              /* CPMU RTI Control Register; 0x0000003B */
volatile CPMUCOPSTR _CPMUCOP;                              /* CPMU COP Control Register; 0x0000003C */
volatile CPMUARMCOPSTR _CPMUARMCOP;                        /* CPMU COP Timer Arm/Reset Register; 0x0000003F */
volatile TIOSSTR _TIOS;                                    /* Timer Input Capture/Output Compare Select; 0x00000040 */
volatile CFORCSTR _CFORC;                                  /* Timer Compare Force Register; 0x00000041 */
volatile TSCR1STR _TSCR1;                                  /* Timer System Control Register1; 0x00000046 */
volatile TTOVSTR _TTOV;                                    /* Timer Toggle On Overflow Register; 0x00000047 */
volatile TCTL1STR _TCTL1;                                  /* Timer Control Register 1; 0x00000048 */
volatile TCTL2STR _TCTL2;                                  /* Timer Control Register 2; 0x00000049 */
volatile TCTL3STR _TCTL3;                                  /* Timer Control Register 3; 0x0000004A */
volatile TCTL4STR _TCTL4;                                  /* Timer Control Register 4; 0x0000004B */
volatile TIESTR _TIE;                                      /* Timer Interrupt Enable Register; 0x0000004C */
volatile TSCR2STR _TSCR2;                                  /* Timer System Control Register 2; 0x0000004D */
volatile TFLG1STR _TFLG1;                                  /* Main Timer Interrupt Flag 1; 0x0000004E */
volatile TFLG2STR _TFLG2;                                  /* Main Timer Interrupt Flag 2; 0x0000004F */
volatile OCPDSTR _OCPD;                                    /* Output Compare Pin Disconnect Register; 0x0000006C */
volatile PTPSRSTR _PTPSR;                                  /* Precision Timer Prescaler Select Register; 0x0000006E */
volatile ATDSTAT0STR _ATDSTAT0;                            /* ATD  Status Register 0; 0x00000076 */
volatile PWMESTR _PWME;                                    /* PWM Enable Register; 0x000000A0 */
volatile PWMPOLSTR _PWMPOL;                                /* PWM Polarity Register; 0x000000A1 */
volatile PWMCLKSTR _PWMCLK;                                /* PWM Clock Select Register; 0x000000A2 */
volatile PWMPRCLKSTR _PWMPRCLK;                            /* PWM Prescale Clock Select Register; 0x000000A3 */
volatile PWMCAESTR _PWMCAE;                                /* PWM Center Align Enable Register; 0x000000A4 */
volatile PWMCTLSTR _PWMCTL;                                /* PWM Control Register; 0x000000A5 */
volatile PWMCLKABSTR _PWMCLKAB;                            /* PWM Clock Select Register; 0x000000A6 */
volatile PWMSCLASTR _PWMSCLA;                              /* PWM Scale A Register; 0x000000A8 */
volatile PWMSCLBSTR _PWMSCLB;                              /* PWM Scale B Register; 0x000000A9 */
volatile SCI0ACR2STR _SCI0ACR2;                            /* SCI 0 Alternative Control Register 2; 0x000000CA */
volatile SCI0CR2STR _SCI0CR2;                              /* SCI 0 Control Register 2; 0x000000CB */
volatile SCI0SR1STR _SCI0SR1;                              /* SCI 0 Status Register 1; 0x000000CC */
volatile SCI0SR2STR _SCI0SR2;                              /* SCI 0 Status Register 2; 0x000000CD */
volatile SCI0DRHSTR _SCI0DRH;                              /* SCI 0 Data Register High; 0x000000CE */
volatile SCI0DRLSTR _SCI0DRL;                              /* SCI 0 Data Register Low; 0x000000CF */
volatile SPI0CR1STR _SPI0CR1;                              /* SPI 0 Control Register 1; 0x000000D8 */
volatile SPI0CR2STR _SPI0CR2;                              /* SPI 0 Control Register 2; 0x000000D9 */
volatile SPI0BRSTR _SPI0BR;                                /* SPI 0 Baud Rate Register; 0x000000DA */
volatile SPI0SRSTR _SPI0SR;                                /* SPI 0 Status Register; 0x000000DB */
volatile FCLKDIVSTR _FCLKDIV;                              /* Flash Clock Divider Register; 0x00000100 */
volatile FSECSTR _FSEC;                                    /* Flash Security Register; 0x00000101 */
volatile FCCOBIXSTR _FCCOBIX;                              /* Flash CCOB Index Register; 0x00000102 */
volatile FCNFGSTR _FCNFG;                                  /* Flash Configuration Register; 0x00000104 */
volatile FERCNFGSTR _FERCNFG;                              /* Flash Error Configuration Register; 0x00000105 */
volatile FSTATSTR _FSTAT;                                  /* Flash Status Register; 0x00000106 */
volatile FERSTATSTR _FERSTAT;                              /* Flash Error Status Register; 0x00000107 */
volatile FPROTSTR _FPROT;                                  /* P-Flash Protection Register; 0x00000108 */
volatile DFPROTSTR _DFPROT;                                /* D-Flash Protection Register; 0x00000109 */
volatile FOPTSTR _FOPT;                                    /* Flash Option Register; 0x00000110 */
volatile IVBRSTR _IVBR;                                    /* Interrupt Vector Base Register; 0x00000120 */
volatile PTTSTR _PTT;                                      /* Port T Data Register; 0x00000240 */
volatile PTITSTR _PTIT;                                    /* Port T Input Register; 0x00000241 */
volatile DDRTSTR _DDRT;                                    /* Port T Data Direction Register; 0x00000242 */
volatile PERTSTR _PERT;                                    /* Port T Pull Device Enable Register; 0x00000244 */
volatile PPSTSTR _PPST;                                    /* Port T Polarity Select Register; 0x00000245 */
volatile PTSSTR _PTS;                                      /* Port S Data Register; 0x00000248 */
volatile PTISSTR _PTIS;                                    /* Port S Input Register; 0x00000249 */
volatile DDRSSTR _DDRS;                                    /* Port S Data Direction Register; 0x0000024A */
volatile PERSSTR _PERS;                                    /* Port S Pull Device Enable Register; 0x0000024C */
volatile PPSSSTR _PPSS;                                    /* Port S Polarity Select Register; 0x0000024D */
volatile WOMSSTR _WOMS;                                    /* Port S Wired-Or Mode Register; 0x0000024E */
volatile PRR0STR _PRR0;                                    /* Pin Routing Register 0; 0x0000024F */
volatile PTMSTR _PTM;                                      /* Port M Data Register; 0x00000250 */
volatile PTIMSTR _PTIM;                                    /* Port M Input Register; 0x00000251 */
volatile DDRMSTR _DDRM;                                    /* Port M Data Direction Register; 0x00000252 */
volatile PERMSTR _PERM;                                    /* Port M Pull Device Enable Register; 0x00000254 */
volatile PPSMSTR _PPSM;                                    /* Port M Polarity Select Register; 0x00000255 */
volatile WOMMSTR _WOMM;                                    /* Port M Wired-Or Mode Register; 0x00000256 */
volatile PKGCRSTR _PKGCR;                                  /* Package Code Register; 0x00000257 */
volatile PTPSTR _PTP;                                      /* Port P Data Register; 0x00000258 */
volatile PTIPSTR _PTIP;                                    /* Port P Input Register; 0x00000259 */
volatile DDRPSTR _DDRP;                                    /* Port P Data Direction Register; 0x0000025A */
volatile PERPSTR _PERP;                                    /* Port P Pull Device Enable Register; 0x0000025C */
volatile PPSPSTR _PPSP;                                    /* Port P Polarity Select Register; 0x0000025D */
volatile PIEPSTR _PIEP;                                    /* Port P Interrupt Enable Register; 0x0000025E */
volatile PIFPSTR _PIFP;                                    /* Port P Interrupt Flag Register; 0x0000025F */
volatile ACMPCSTR _ACMPC;                                  /* ACMP control register; 0x00000260 */
volatile ACMPSSTR _ACMPS;                                  /* ACMP status register; 0x00000261 */
volatile PTJSTR _PTJ;                                      /* Port J Data Register; 0x00000268 */
volatile PTIJSTR _PTIJ;                                    /* Port J Input Register; 0x00000269 */
volatile DDRJSTR _DDRJ;                                    /* Port J Data Direction Register; 0x0000026A */
volatile PERJSTR _PERJ;                                    /* Port J Pull Device Enable Register; 0x0000026C */
volatile PPSJSTR _PPSJ;                                    /* Port J Polarity Select Register; 0x0000026D */
volatile PIEJSTR _PIEJ;                                    /* Port J Interrupt Enable Register; 0x0000026E */
volatile PIFJSTR _PIFJ;                                    /* Port J Interrupt Flag Register; 0x0000026F */
volatile CPMULVCTLSTR _CPMULVCTL;                          /* Low Voltage Control Register; 0x000002F1 */
volatile CPMUAPICTLSTR _CPMUAPICTL;                        /* Autonomous Periodical Interrupt Control Register; 0x000002F2 */
volatile CPMUACLKTRSTR _CPMUACLKTR;                        /* Autonomous Clock Trimming Register; 0x000002F3 */
volatile CPMUOSCSTR _CPMUOSC;                              /* S12CPMU Oscillator Register; 0x000002FA */
volatile CPMUPROTSTR _CPMUPROT;                            /* S12CPMUV1 Protection Register; 0x000002FB */
/* NVFPROT - macro for reading non volatile register       Non Volatile P-Flash Protection Register; 0x0000FF0C */
/* NVDFPROT - macro for reading non volatile register      Non Volatile D-Flash Protection Register; 0x0000FF0D */
/* NVFOPT - macro for reading non volatile register        Non Volatile Flash Option Register; 0x0000FF0E */
/* NVFSEC - macro for reading non volatile register        Non Volatile Flash Security Register; 0x0000FF0F */


/* * * * *  16-BIT REGISTERS  * * * * * * * * * * * * * * * */
volatile PARTIDSTR _PARTID;                                /* Part ID Register; 0x0000001A */
volatile DBGTBSTR _DBGTB;                                  /* Debug Trace Buffer Register; 0x00000024 */
volatile TCNTSTR _TCNT;                                    /* Timer Count Register; 0x00000044 */
volatile TC0STR _TC0;                                      /* Timer Input Capture/Output Compare Register 0; 0x00000050 */
volatile TC1STR _TC1;                                      /* Timer Input Capture/Output Compare Register 1; 0x00000052 */
volatile TC2STR _TC2;                                      /* Timer Input Capture/Output Compare Register 2; 0x00000054 */
volatile TC3STR _TC3;                                      /* Timer Input Capture/Output Compare Register 3; 0x00000056 */
volatile TC4STR _TC4;                                      /* Timer Input Capture/Output Compare Register 4; 0x00000058 */
volatile TC5STR _TC5;                                      /* Timer Input Capture/Output Compare Register 5; 0x0000005A */
volatile ATDCTL01STR _ATDCTL01;                            /* ATD  Control Register 01; 0x00000070 */
volatile ATDCTL23STR _ATDCTL23;                            /* ATD  Control Register 23; 0x00000072 */
volatile ATDCTL45STR _ATDCTL45;                            /* ATD  Control Register 45; 0x00000074 */
volatile ATDCMPESTR _ATDCMPE;                              /* ATD  Compare Enable Register; 0x00000078 */
volatile ATDSTAT2STR _ATDSTAT2;                            /* ATD  Status Register 2; 0x0000007A */
volatile ATDDIENSTR _ATDDIEN;                              /* ATD  Input Enable Register; 0x0000007C */
volatile ATDCMPHTSTR _ATDCMPHT;                            /* ATD  Compare Higher Than Register; 0x0000007E */
volatile ATDDR0STR _ATDDR0;                                /* ATD  Conversion Result Register 0; 0x00000080 */
volatile ATDDR1STR _ATDDR1;                                /* ATD  Conversion Result Register 1; 0x00000082 */
volatile ATDDR2STR _ATDDR2;                                /* ATD  Conversion Result Register 2; 0x00000084 */
volatile ATDDR3STR _ATDDR3;                                /* ATD  Conversion Result Register 3; 0x00000086 */
volatile ATDDR4STR _ATDDR4;                                /* ATD  Conversion Result Register 4; 0x00000088 */
volatile ATDDR5STR _ATDDR5;                                /* ATD  Conversion Result Register 5; 0x0000008A */
volatile ATDDR6STR _ATDDR6;                                /* ATD  Conversion Result Register 6; 0x0000008C */
volatile ATDDR7STR _ATDDR7;                                /* ATD  Conversion Result Register 7; 0x0000008E */
volatile PWMCNT01STR _PWMCNT01;                            /* PWM Channel Counter 01 Register; 0x000000AC */
volatile PWMCNT23STR _PWMCNT23;                            /* PWM Channel Counter 23 Register; 0x000000AE */
volatile PWMCNT45STR _PWMCNT45;                            /* PWM Channel Counter 45 Register; 0x000000B0 */
volatile PWMPER01STR _PWMPER01;                            /* PWM Channel Period 01 Register; 0x000000B4 */
volatile PWMPER23STR _PWMPER23;                            /* PWM Channel Period 23 Register; 0x000000B6 */
volatile PWMPER45STR _PWMPER45;                            /* PWM Channel Period 45 Register; 0x000000B8 */
volatile PWMDTY01STR _PWMDTY01;                            /* PWM Channel Duty 01 Register; 0x000000BC */
volatile PWMDTY23STR _PWMDTY23;                            /* PWM Channel Duty 23 Register; 0x000000BE */
volatile PWMDTY45STR _PWMDTY45;                            /* PWM Channel Duty 45 Register; 0x000000C0 */
volatile SCI0BDSTR _SCI0BD;                                /* SCI 0 Baud Rate Register; 0x000000C8 */
volatile SPI0DRSTR _SPI0DR;                                /* SPI 0 Data Register; 0x000000DC */
volatile FCCOBSTR _FCCOB;                                  /* Flash Common Command Object Register; 0x0000010A */
volatile PT01ADSTR _PT01AD;                                /* Port AD Data Register; 0x00000270 */
volatile PTI01ADSTR _PTI01AD;                              /* Port AD Input Register; 0x00000272 */
volatile DDR01ADSTR _DDR01AD;                              /* Port AD Data Direction Register; 0x00000274 */
volatile PER01ADSTR _PER01AD;                              /* Port AD Pull Up Enable Register; 0x00000278 */
volatile PPS01ADSTR _PPS01AD;                              /* Port AD Polarity Select Register; 0x0000027A */
volatile PIE01ADSTR _PIE01AD;                              /* Port AD Interrupt Enable Register; 0x0000027C */
volatile PIF01ADSTR _PIF01AD;                              /* Port AD Interrupt Flag Register; 0x0000027E */
volatile CPMUAPIRSTR _CPMUAPIR;                            /* Autonomous Periodical Interrupt Rate Register; 0x000002F4 */
volatile CPMUIRCTRIMSTR _CPMUIRCTRIM;                      /* S12CPMU IRC1M Trim Registers; 0x000002F8 */
/* BAKEY0 - macro for reading non volatile register        Backdoor Comparison Key 0; 0x0000FF00 */
/* BAKEY1 - macro for reading non volatile register        Backdoor Comparison Key 1; 0x0000FF02 */
/* BAKEY2 - macro for reading non volatile register        Backdoor Comparison Key 2; 0x0000FF04 */
/* BAKEY3 - macro for reading non volatile register        Backdoor Comparison Key 3; 0x0000FF06 */

/*lint -restore */

/* EOF */
