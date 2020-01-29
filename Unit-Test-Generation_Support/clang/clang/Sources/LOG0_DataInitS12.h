#ifndef LOG_DATA_INIT_S12_H
#define LOG_DATA_INIT_S12_H

#define LOGDATA_INIT1 \
{                                                                                               \
  0xF0, 0xFF, 0xFF, /* Orologio   */                                                            \
  /*attenzione LSB a sinistra MSB a destra es: 00 00 01 = 65536 dec*/                           \
  0x04,             /* Tmax TLE*/                                                               \
  0x05,             /* Tmax MOSFET*/                                                            \
  0x05, 0x06, 0x07, /* Salvataggi*/                                                             \
  0xF8, 0x01, 0x02, 0x03, 0x04, 0x05, 0x05, 0x05,                 /* Conta Errori*/             \
  0x06, 0x07, 0x08,       0x01, 0x02, 0x03,     0x04, 0x05, 0x05, /* LogT 50-90, 90-125, 125->*/\
  0x06, 0x07, 0x08,       0x01, 0x02, 0x03,                       /* LogV 50-80, 80->*/         \
                                                                                                \
  0x00,       /* Codice*/                                                                       \
  0x00,       /* u16 Tempo, solo 1 inizializzatore !!! e' u16 !!!*/                             \
  0x00,       /* Tensione*/                                                                     \
  0x00,       /* Temperatura*/                                                                  \
  0x00,       /* Velocita*/                                                                     \
  0x00,       /* Tensione*/                                                                     \
                                                                                                \
                                                                                                \
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Free*/                                                 \
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Free*/                                                 \
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Free*/                                                 \
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Free*/                                                 \
  0x00,  /* Dati Integri*/                                                                      \
  0x0E /* Checksum*/                                                                            \
};  

#define LOGDATA_INIT2  \
{                                        \
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, \
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, \
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, \
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, \
0xFFFF   ,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, \
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, \
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, \
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF  \
};

#define LOGDATA_INIT3 \
{                                        \
  0x00, 0x00, 0x01, /* Orologio */       \
  /*attenzione LSB a sinistra MSB a destra es: 00 00 01 = 65536 dec*/                           \
  0x04,             /* Tmax TLE */       \
  0x05,             /* Tmax MOSFET */    \
  0x05, 0x06, 0x07, /* Salvataggi */     \
  0xF8, 0x01, 0x02, 0x03, 0x04, 0x05, 0x05, 0x05,                 /* Conta Errori */                \
  0x06, 0x07, 0x08,       0x01, 0x02, 0x03,     0x04, 0x05, 0x05, /* LogT 50-90, 90-125, 125->*/    \
  0xF0, 0xFF, 0xFF,       0x01, 0x02, 0x03,                       /* LogV 50-80, 80-> */            \
\
  0x00,       /* Codice */                                              \
  0x00,       /* u16 Tempo, solo 1 inizializzatore !!! e' u16 !!! */    \
  0x00,       /* Tensione */                                            \
  0x00,       /* Temperatura */                                         \
  0x00,       /* Velocita */                                            \
  0x00,       /* Tensione */                                            \
                                                                        \
                                                                        \
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Free */                        \
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Free */                        \
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Free */                        \
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Free */                        \
  0x00,  /* Dati Integri */                                             \
  0x27 /* Checksum */                                                   \
};
#endif /* LOG_DATA_INIT_S12_H */
