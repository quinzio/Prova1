/******************************************************************************
  FILE        : checksum.c
  PURPOSE     : generic checksum utility functions
  LANGUAGE    : ANSI-C
  -----------------------------------------------------------------------------
  HISTORY
    19 sep 2002       Created.
  General notes:
  We do define here more functions than anyone would like to use at one time.
  Or in other words, we rely on the linker to drop the non referenced functions.
  So don't switch of smart linking for this module.
 ******************************************************************************/

#ifdef _CANTATA_
#include "cantata.h"
#endif
#include "SPAL_Setup.h"
#include MICRO_REGISTERS
#include SPAL_DEF
#include MAC0_Register      // Needed for Watchdog
#include "WTD1_Watchdog.h"
#include "checksum.h"

#ifndef __HIWARE__ /* HIWARE target compilers are using the tables, any host compiler computes them */
#define ENABLE_CALCULATE_TABLES       1 /* calculate the precomputed tables */
#else
#define ENABLE_CALCULATE_TABLES       0
#endif

#ifndef ENABLE_PRECOMPUTE
#define ENABLE_PRECOMPUTE (!ENABLE_CALCULATE_TABLES && 1) /* 1: use tables: much faster, but needs more ROM */
#endif

#if ENABLE_PRECOMPUTE

/* Description:
The CRC algorithm is in its generic form is a loop over 8 bits where in every loop
  a 1 bit shift is performed. If in this shift a one bit drops out of the value,
  a number (the polynom) is xored into the value.
  Because of this, only the 8 leading bits and the polynom control
  when to which positions the polynom is xorded. The tables below just
  precompute this for all 256 values of the leading byte.
  These tables are therefore only usable for a specific polynom
  (the ones called default in checksum.h). But at the bottom of this
  file is the code to compute these tables. This code can easily be
  adapted for different polynoms.
*/

/* CRC8 */
static const _CheckSum1ByteType TableCRC8[256]= {
/*            0,     1,     2,     3,     4,     5,     6,     7,*/
/*   0 */     0,   155,   173,    54,   193,    90,   108,   247,
/*   8 */    25,   130,   180,    47,   216,    67,   117,   238,
/*  16 */    50,   169,   159,     4,   243,   104,    94,   197,
/*  24 */    43,   176,   134,    29,   234,   113,    71,   220,
/*  32 */   100,   255,   201,    82,   165,    62,     8,   147,
/*  40 */   125,   230,   208,    75,   188,    39,    17,   138,
/*  48 */    86,   205,   251,    96,   151,    12,    58,   161,
/*  56 */    79,   212,   226,   121,   142,    21,    35,   184,
/*  64 */   200,    83,   101,   254,     9,   146,   164,    63,
/*  72 */   209,    74,   124,   231,    16,   139,   189,    38,
/*  80 */   250,    97,    87,   204,    59,   160,   150,    13,
/*  88 */   227,   120,    78,   213,    34,   185,   143,    20,
/*  96 */   172,    55,     1,   154,   109,   246,   192,    91,
/* 104 */   181,    46,    24,   131,   116,   239,   217,    66,
/* 112 */   158,     5,    51,   168,    95,   196,   242,   105,
/* 120 */   135,    28,    42,   177,    70,   221,   235,   112,
/* 128 */    11,   144,   166,    61,   202,    81,   103,   252,
/* 136 */    18,   137,   191,    36,   211,    72,   126,   229,
/* 144 */    57,   162,   148,    15,   248,    99,    85,   206,
/* 152 */    32,   187,   141,    22,   225,   122,    76,   215,
/* 160 */   111,   244,   194,    89,   174,    53,     3,   152,
/* 168 */   118,   237,   219,    64,   183,    44,    26,   129,
/* 176 */    93,   198,   240,   107,   156,     7,    49,   170,
/* 184 */    68,   223,   233,   114,   133,    30,    40,   179,
/* 192 */   195,    88,   110,   245,     2,   153,   175,    52,
/* 200 */   218,    65,   119,   236,    27,   128,   182,    45,
/* 208 */   241,   106,    92,   199,    48,   171,   157,     6,
/* 216 */   232,   115,    69,   222,    41,   178,   132,    31,
/* 224 */   167,    60,    10,   145,   102,   253,   203,    80,
/* 232 */   190,    37,    19,   136,   127,   228,   210,    73,
/* 240 */   149,    14,    56,   163,    84,   207,   249,    98,
/* 248 */   140,    23,    33,   186,    77,   214,   224,   123,
};

/* CRC16 */
static const _CheckSum2ByteType TableCRC16[256]= {
/*            0,     1,     2,     3,     4,     5,     6,     7,*/
/*   0 */     0, 32773, 32783,    10, 32795,    30,    20, 32785,
/*   8 */ 32819,    54,    60, 32825,    40, 32813, 32807,    34,
/*  16 */ 32867,   102,   108, 32873,   120, 32893, 32887,   114,
/*  24 */    80, 32853, 32863,    90, 32843,    78,    68, 32833,
/*  32 */ 32963,   198,   204, 32969,   216, 32989, 32983,   210,
/*  40 */   240, 33013, 33023,   250, 33003,   238,   228, 32993,
/*  48 */   160, 32933, 32943,   170, 32955,   190,   180, 32945,
/*  56 */ 32915,   150,   156, 32921,   136, 32909, 32903,   130,
/*  64 */ 33155,   390,   396, 33161,   408, 33181, 33175,   402,
/*  72 */   432, 33205, 33215,   442, 33195,   430,   420, 33185,
/*  80 */   480, 33253, 33263,   490, 33275,   510,   500, 33265,
/*  88 */ 33235,   470,   476, 33241,   456, 33229, 33223,   450,
/*  96 */   320, 33093, 33103,   330, 33115,   350,   340, 33105,
/* 104 */ 33139,   374,   380, 33145,   360, 33133, 33127,   354,
/* 112 */ 33059,   294,   300, 33065,   312, 33085, 33079,   306,
/* 120 */   272, 33045, 33055,   282, 33035,   270,   260, 33025,
/* 128 */ 33539,   774,   780, 33545,   792, 33565, 33559,   786,
/* 136 */   816, 33589, 33599,   826, 33579,   814,   804, 33569,
/* 144 */   864, 33637, 33647,   874, 33659,   894,   884, 33649,
/* 152 */ 33619,   854,   860, 33625,   840, 33613, 33607,   834,
/* 160 */   960, 33733, 33743,   970, 33755,   990,   980, 33745,
/* 168 */ 33779,  1014,  1020, 33785,  1000, 33773, 33767,   994,
/* 176 */ 33699,   934,   940, 33705,   952, 33725, 33719,   946,
/* 184 */   912, 33685, 33695,   922, 33675,   910,   900, 33665,
/* 192 */   640, 33413, 33423,   650, 33435,   670,   660, 33425,
/* 200 */ 33459,   694,   700, 33465,   680, 33453, 33447,   674,
/* 208 */ 33507,   742,   748, 33513,   760, 33533, 33527,   754,
/* 216 */   720, 33493, 33503,   730, 33483,   718,   708, 33473,
/* 224 */ 33347,   582,   588, 33353,   600, 33373, 33367,   594,
/* 232 */   624, 33397, 33407,   634, 33387,   622,   612, 33377,
/* 240 */   544, 33317, 33327,   554, 33339,   574,   564, 33329,
/* 248 */ 33299,   534,   540, 33305,   520, 33293, 33287,   514,
};

/* CRC32 */
static const _CheckSum4ByteType TableCRC32[256]= {
/*                 0,          1,          2,          3,*/
/*   0 */          0u,   79764919u,  159529838u,  222504665u,
/*   4 */  319059676u,  398814059u,  445009330u,  507990021u,
/*   8 */  638119352u,  583659535u,  797628118u,  726387553u,
/*  12 */  890018660u,  835552979u, 1015980042u,  944750013u,
/*  16 */ 1276238704u, 1221641927u, 1167319070u, 1095957929u,
/*  20 */ 1595256236u, 1540665371u, 1452775106u, 1381403509u,
/*  24 */ 1780037320u, 1859660671u, 1671105958u, 1733955601u,
/*  28 */ 2031960084u, 2111593891u, 1889500026u, 1952343757u,
/*  32 */ 2552477408u, 2632100695u, 2443283854u, 2506133561u,
/*  36 */ 2334638140u, 2414271883u, 2191915858u, 2254759653u,
/*  40 */ 3190512472u, 3135915759u, 3081330742u, 3009969537u,
/*  44 */ 2905550212u, 2850959411u, 2762807018u, 2691435357u,
/*  48 */ 3560074640u, 3505614887u, 3719321342u, 3648080713u,
/*  52 */ 3342211916u, 3287746299u, 3467911202u, 3396681109u,
/*  56 */ 4063920168u, 4143685023u, 4223187782u, 4286162673u,
/*  60 */ 3779000052u, 3858754371u, 3904687514u, 3967668269u,
/*  64 */  881225847u,  809987520u, 1023691545u,  969234094u,
/*  68 */  662832811u,  591600412u,  771767749u,  717299826u,
/*  72 */  311336399u,  374308984u,  453813921u,  533576470u,
/*  76 */   25881363u,   88864420u,  134795389u,  214552010u,
/*  80 */ 2023205639u, 2086057648u, 1897238633u, 1976864222u,
/*  84 */ 1804852699u, 1867694188u, 1645340341u, 1724971778u,
/*  88 */ 1587496639u, 1516133128u, 1461550545u, 1406951526u,
/*  92 */ 1302016099u, 1230646740u, 1142491917u, 1087903418u,
/*  96 */ 2896545431u, 2825181984u, 2770861561u, 2716262478u,
/* 100 */ 3215044683u, 3143675388u, 3055782693u, 3001194130u,
/* 104 */ 2326604591u, 2389456536u, 2200899649u, 2280525302u,
/* 108 */ 2578013683u, 2640855108u, 2418763421u, 2498394922u,
/* 112 */ 3769900519u, 3832873040u, 3912640137u, 3992402750u,
/* 116 */ 4088425275u, 4151408268u, 4197601365u, 4277358050u,
/* 120 */ 3334271071u, 3263032808u, 3476998961u, 3422541446u,
/* 124 */ 3585640067u, 3514407732u, 3694837229u, 3640369242u,
/* 128 */ 1762451694u, 1842216281u, 1619975040u, 1682949687u,
/* 132 */ 2047383090u, 2127137669u, 1938468188u, 2001449195u,
/* 136 */ 1325665622u, 1271206113u, 1183200824u, 1111960463u,
/* 140 */ 1543535498u, 1489069629u, 1434599652u, 1363369299u,
/* 144 */  622672798u,  568075817u,  748617968u,  677256519u,
/* 148 */  907627842u,  853037301u, 1067152940u,  995781531u,
/* 152 */   51762726u,  131386257u,  177728840u,  240578815u,
/* 156 */  269590778u,  349224269u,  429104020u,  491947555u,
/* 160 */ 4046411278u, 4126034873u, 4172115296u, 4234965207u,
/* 164 */ 3794477266u, 3874110821u, 3953728444u, 4016571915u,
/* 168 */ 3609705398u, 3555108353u, 3735388376u, 3664026991u,
/* 172 */ 3290680682u, 3236090077u, 3449943556u, 3378572211u,
/* 176 */ 3174993278u, 3120533705u, 3032266256u, 2961025959u,
/* 180 */ 2923101090u, 2868635157u, 2813903052u, 2742672763u,
/* 184 */ 2604032198u, 2683796849u, 2461293480u, 2524268063u,
/* 188 */ 2284983834u, 2364738477u, 2175806836u, 2238787779u,
/* 192 */ 1569362073u, 1498123566u, 1409854455u, 1355396672u,
/* 196 */ 1317987909u, 1246755826u, 1192025387u, 1137557660u,
/* 200 */ 2072149281u, 2135122070u, 1912620623u, 1992383480u,
/* 204 */ 1753615357u, 1816598090u, 1627664531u, 1707420964u,
/* 208 */  295390185u,  358241886u,  404320391u,  483945776u,
/* 212 */   43990325u,  106832002u,  186451547u,  266083308u,
/* 216 */  932423249u,  861060070u, 1041341759u,  986742920u,
/* 220 */  613929101u,  542559546u,  756411363u,  701822548u,
/* 224 */ 3316196985u, 3244833742u, 3425377559u, 3370778784u,
/* 228 */ 3601682597u, 3530312978u, 3744426955u, 3689838204u,
/* 232 */ 3819031489u, 3881883254u, 3928223919u, 4007849240u,
/* 236 */ 4037393693u, 4100235434u, 4180117107u, 4259748804u,
/* 240 */ 2310601993u, 2373574846u, 2151335527u, 2231098320u,
/* 244 */ 2596047829u, 2659030626u, 2470359227u, 2550115596u,
/* 248 */ 2947551409u, 2876312838u, 2788305887u, 2733848168u,
/* 252 */ 3165939309u, 3094707162u, 3040238851u, 2985771188u,
};

/* CRC_CCITT */
static const _CheckSum2ByteType Table_CCITT[256]= {
/*            0,     1,     2,     3,     4,     5,     6,     7,*/
/*   0 */     0,  4129,  8258, 12387, 16516, 20645, 24774, 28903,
/*   8 */ 33032, 37161, 41290, 45419, 49548, 53677, 57806, 61935,
/*  16 */  4657,   528, 12915,  8786, 21173, 17044, 29431, 25302,
/*  24 */ 37689, 33560, 45947, 41818, 54205, 50076, 62463, 58334,
/*  32 */  9314, 13379,  1056,  5121, 25830, 29895, 17572, 21637,
/*  40 */ 42346, 46411, 34088, 38153, 58862, 62927, 50604, 54669,
/*  48 */ 13907,  9842,  5649,  1584, 30423, 26358, 22165, 18100,
/*  56 */ 46939, 42874, 38681, 34616, 63455, 59390, 55197, 51132,
/*  64 */ 18628, 22757, 26758, 30887,  2112,  6241, 10242, 14371,
/*  72 */ 51660, 55789, 59790, 63919, 35144, 39273, 43274, 47403,
/*  80 */ 23285, 19156, 31415, 27286,  6769,  2640, 14899, 10770,
/*  88 */ 56317, 52188, 64447, 60318, 39801, 35672, 47931, 43802,
/*  96 */ 27814, 31879, 19684, 23749, 11298, 15363,  3168,  7233,
/* 104 */ 60846, 64911, 52716, 56781, 44330, 48395, 36200, 40265,
/* 112 */ 32407, 28342, 24277, 20212, 15891, 11826,  7761,  3696,
/* 120 */ 65439, 61374, 57309, 53244, 48923, 44858, 40793, 36728,
/* 128 */ 37256, 33193, 45514, 41451, 53516, 49453, 61774, 57711,
/* 136 */  4224,   161, 12482,  8419, 20484, 16421, 28742, 24679,
/* 144 */ 33721, 37784, 41979, 46042, 49981, 54044, 58239, 62302,
/* 152 */   689,  4752,  8947, 13010, 16949, 21012, 25207, 29270,
/* 160 */ 46570, 42443, 38312, 34185, 62830, 58703, 54572, 50445,
/* 168 */ 13538,  9411,  5280,  1153, 29798, 25671, 21540, 17413,
/* 176 */ 42971, 47098, 34713, 38840, 59231, 63358, 50973, 55100,
/* 184 */  9939, 14066,  1681,  5808, 26199, 30326, 17941, 22068,
/* 192 */ 55628, 51565, 63758, 59695, 39368, 35305, 47498, 43435,
/* 200 */ 22596, 18533, 30726, 26663,  6336,  2273, 14466, 10403,
/* 208 */ 52093, 56156, 60223, 64286, 35833, 39896, 43963, 48026,
/* 216 */ 19061, 23124, 27191, 31254,  2801,  6864, 10931, 14994,
/* 224 */ 64814, 60687, 56684, 52557, 48554, 44427, 40424, 36297,
/* 232 */ 31782, 27655, 23652, 19525, 15522, 11395,  7392,  3265,
/* 240 */ 61215, 65342, 53085, 57212, 44955, 49082, 36825, 40952,
/* 248 */ 28183, 32310, 20053, 24180, 11923, 16050,  3793,  7920,
};


#endif /* ENABLE_PRECOMPUTE */

/*****************************************************************************/
/********* Utility functions treating 1 byte only for CRC checksums **********/
/*****************************************************************************/

/*PRQA S 1338 ++ #legacy code - freescale library*/
/* CheckSumByteCRC_CCITT: calculate the CRC_CCITT checksum of a single additional byte */

static _CheckSum2ByteType CheckSumByteCRC_CCITT(_CheckSum2ByteType crc, _CheckSum1ByteType val) {
    _CheckSum1ByteType temp, temp1;
    _CheckSum2ByteType quick, quick1;
    temp = (_CheckSum1ByteType)((crc >> 8) ^ val);
    temp1= (_CheckSum1ByteType)crc;
    crc = (_CheckSum2ByteType)((_CheckSum2ByteType)temp1 << 8u);
    quick = (_CheckSum2ByteType)((_CheckSum2ByteType)temp ^ ((_CheckSum2ByteType)temp >> 4u));
    crc ^= quick;
#if 1 /* faster to use shifts by 4/8 */
    quick1 = quick << 4;
    crc = crc ^(quick1 << 1);
    return (crc ^ (quick1 << 8));
#else
    quick <<= 5;
    crc ^= quick;
    quick <<= 7;
    crc ^= quick;
    return crc;
#endif
}
/*PRQA S 1338 -- #legacy code - freescale library*/


#if ENABLE_CALCULATE_TABLES

static void ComputeCRC_CCITT_PreCalc(_CheckSum2ByteType* table) {
  _CheckSum1ByteType index= 0;
  do {
    _CheckSum2ByteType val= CheckSumByteCRC_CCITT((_CheckSum2ByteType)(index << 8), 0);
    table[index]= val;
    index++;
  } while ((index & 0xff) != 0); /* overflow */
}

static void ComputeCRC8_PreCalc(_CheckSum1ByteType* table, _CheckSum1ByteType poly) {
  _CheckSum1ByteType index= 0;
  do {
    _CheckSum1ByteType val= CheckSumByteCRC8((_CheckSum1ByteType)index, 0, poly);
    table[index]= val;
    index++;
  } while ((index & 0xff) != 0); /* overflow */
}

static void ComputeCRC16_PreCalc(_CheckSum2ByteType* table, _CheckSum2ByteType poly) {
  _CheckSum1ByteType index= 0;
  do {
    _CheckSum2ByteType val= CheckSumByteCRC16((_CheckSum2ByteType)(index << 8), 0, poly);
    table[index]= val;
    index++;
  } while ((index & 0xff) != 0); /* overflow */
}

static void ComputeCRC32_PreCalc(_CheckSum4ByteType* table, _CheckSum4ByteType poly) {
  _CheckSum1ByteType index= 0;
  do {
    _CheckSum4ByteType val= CheckSumByteCRC32((_CheckSum4ByteType)index << 24, 0, poly);
    table[index]= val;
    index++;
  } while ((index & 0xff) != 0); /* overflow */
}

#endif /* ENABLE_CALCULATE_TABLES */

/*****************************************************************************/
/********* Utility functions calculating the checksum of an area. ************/
/*****************************************************************************/



/*****************************************************************************/
/********* Utility functions calculating the single checksum for multiple memory areas. ************/
/*****************************************************************************/

/* _Checksum_CheckAreasCRC_CCITT: CRC CCITT checksum of multiple mem areas */

_CheckSum2ByteType _Checksum_CheckAreasCRC_CCITT(const struct __ChecksumArea * areas,int num,_CheckSum2ByteType init ) {
  /* ITU-TSS CRC for polynom G(x)=x**16+x**12+x**5+1  ("**" is for exponentiation) */
  _CheckSum2ByteType crc = init;
  int i;
  /* unsigned int len; */

  for (i=(int)0 ; i < num; i++) {
    const _CheckSum1ByteType* _CHECKSUM_QUALI ptr= (const _CheckSum1ByteType* _CHECKSUM_QUALI)areas[i].start;
    const _CheckSum1ByteType* _CHECKSUM_QUALI endPtr= ptr + areas[i].len; /*PRQA S 0488 #This pointer arithmetic is correct*/
    /* len = areas[i].len; */
    while (ptr != endPtr) {
      crc= CheckSumByteCRC_CCITT(crc, *ptr);
      WTD_ResetWatchdog();     /* COP enabled? If so, feed it */
      ptr++;
    }
  }
  return crc; /* == check->checkSumCRC_CCITT */
}


#if ENABLE_CALCULATE_TABLES
/* code which calculates and prints the tables above to stdout */
/* This code is intended for a host C compiler */

#include <stdio.h>

_CheckSum1ByteType TableCRC8[256];
_CheckSum2ByteType TableCRC16[256];
_CheckSum4ByteType TableCRC32[256];
_CheckSum2ByteType Table_CCITT[256];

int main(void) {
  int i;
  const int entriesPerLine16= 8;
  const int entriesPerLine32= 4;
  ComputeCRC8_PreCalc( TableCRC8,  DEFAULT_CRC8_CHECKSUM);
  ComputeCRC16_PreCalc(TableCRC16, DEFAULT_CRC16_CHECKSUM);
  ComputeCRC32_PreCalc(TableCRC32, DEFAULT_CRC32_CHECKSUM);
  ComputeCRC_CCITT_PreCalc(Table_CCITT);

  printf("/* CRC8 */\n");
  printf("static const _CheckSum1ByteType TableCRC8[256]= {\n/*       ");
  for (i= 0; i < entriesPerLine16; i++) {
    printf("%6u,", i);
  }
  printf("*/");
  for (i= 0; i < 256; i++) {
    if (i % entriesPerLine16 == 0) {
      printf("\n/*%4u */", i);
    }
    printf("%6u,", TableCRC8[i]);
  }
  printf("\n};\n\n");

  printf("/* CRC16 */\n");
  printf("static const _CheckSum2ByteType TableCRC16[256]= {\n/*       ");
  for (i= 0; i < entriesPerLine16; i++) {
    printf("%6u,", i);
  }
  printf("*/");
  for (i= 0; i < 256; i++) {
    if (i % entriesPerLine16 == 0) {
      printf("\n/*%4u */", i);
    }
    printf("%6u,", TableCRC16[i]);
  }
  printf("\n};\n\n");

  printf("/* CRC32 */\n");
  printf("static const _CheckSum4ByteType TableCRC32[256]= {\n/*       ");
  for (i= 0; i < entriesPerLine32; i++) {
    printf("%11u,", i);
  }
  printf("*/");
  for (i= 0; i < 256; i++) {
    if (i % entriesPerLine32 == 0) {
      printf("\n/*%4u */", i);
    }
    printf("%11u,", TableCRC32[i]);
  }
  printf("\n};\n\n");

  printf("/* CRC_CCITT */\n");
  printf("static const _CheckSum2ByteType Table_CCITT[256]= {\n/*       ");
  for (i= 0; i < entriesPerLine16; i++) {
    printf("%6u,", i);
  }
  printf("*/");
  for (i= 0; i < 256; i++) {
    if (i % entriesPerLine16 == 0) {
      printf("\n/*%4u */", i);
    }
    printf("%6u,", Table_CCITT[i]);
  }
  printf("\n};\n\n");
  return 0;
}

#endif /* ENABLE_CALCULATE_TABLES */
