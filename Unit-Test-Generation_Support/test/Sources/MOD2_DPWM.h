/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  MOD2_DPWM.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file MOD2_DPWM.h
    \version see \ref FMW_VERSION
    \brief Header with the PWM overmodulation table
    \details This file contains the PWM overmodulation table.
*/

#ifndef DPWM_H
#define DPWM_H   

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

/* Public Constants -------------------------------------------------------- */
/**    
    \def ARRAY_PWM_OVERMOD
    \brief Here is stored the PWM overmodulation table
    \details Due to motor construction, it's needed a sinusoidal delta voltage waveform to drive the motor.
    To do that, the PWM signals that drives the motor have to be drived with a dutycycle waveform that can 
    achive this task. All this has to be done to achive the maximal efficiency.\n
    Sometimes it's accepted to lose a little bit efficiency to improve other parameters: EMI, speed, ....
    So the delta voltage can be distorted and be different from a perfect sinusoid to better achive other result; 
    in this specific case it's introduced an overmodulation done to achive the EMI and speed requirements
    
    \par Overmodulation Starting Requirements
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>

    <em> This reference is done because doxygen can't recognize static variables. The define declared is only
    a dummy reference to document PWM_MOD_3TH[512] </em>
    
    \image rtf PWM_MOD_3TH.JPG "PWM Array Table in overmodulation" width=10cm   
    \image html PWM_MOD_3TH.JPG "PWM Array Table in overmodulation" width=10cm 
    Image from "\\galileo\Elettronica\- SOFTWARE STRUCTURE\DoxyGen Images"
    
    
        
    \note This variable is allocated in the \b FLASH memory
    \warning It's mandatory to be a \b STATIC variable to avoid repetition in code
*/

#define ARRAY_PWM_MOD

/* Public Constants -------------------------------------------------------- */
static const u8 PWM_MOD[256]= /*PRQA S 3218 #Gives better visibility here*/
{
64 ,
67 ,
69 ,
72 ,
74 ,
77 ,
79 ,
82 ,
85 ,
87 ,
90 ,
92 ,
95 ,
97 ,
100,
102,
104,
107,
109,
111,
114,
116,
117,
118,
119,
120,
121,
121,
122,
123,
123,
124,
124,
125,
125,
126,
126,
126,
127,
127,
127,
127,
128,
128,
128,
128,
128,
128,
128,
128,
128,
127,
127,
127,
127,
126,
126,
125,
125,
125,
124,
123,
123,
122,
122,
122,
123,
123,
124,
125,
125,
125,
126,
126,
127,
127,
127,
127,
128,
128,
128,
128,
128,
128,
128,
128,
128,
127,
127,
127,
127,
126,
126,
126,
125,
125,
124,
124,
123,
123,
122,
121,
121,
120,
119,
118,
117,
117,
117,
117,
117,
117,
117,
117,
117,
113,
109,
106,
102,
98 ,
94 ,
91 ,
87 ,
83 ,
79 ,
75 ,
72 ,
68 ,
64 ,
61 ,
57 ,
54 ,
50 ,
47 ,
43 ,
40 ,
36 ,
32 ,
29 ,
25 ,
22 ,
18 ,
14 ,
11 ,
7  ,
6  ,
5  ,
4  ,
3  ,
2  ,
1  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
0  ,
1  ,
3  ,
4  ,
5  ,
6  ,
8  ,
9  ,
14 ,
19 ,
23 ,
28 ,
33 ,
37 ,
42 ,
47 ,
52 ,
56 ,
61
};

/* -------------------------- Public type definition --------------------- */

/* -------------------------- Public Variables --------------------------- */
 
/* -------------------------- Public Functions prototypes ---------------- */

/* -------------------------- Tacho functions ---------------------------- */

/* -------------------------- Low Power function ------------------------- */

/*--------------------------- Timer function ------------------------------*/

/*--------------------------- Gate driver function ------------------------*/

#endif  /* DPWM_H */
/*** (c) 2007 SPAL Automotive ****************** END OF FILE **************/
