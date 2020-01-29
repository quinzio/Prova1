/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  main.h

VERSION  :  1.2

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

/** 
    \file main.h
    \version see \ref FMW_VERSION
    \brief Main file header
    \details This file contains the definition of the state machine enums and a description of the main page documentation.
*/

/** 
    \mainpage SBL 300 DePhi: Main Trunk
    \details Software documentation
    The general documentation, not lied to a particular file, is divede in some paragraph; here there is a brief summary of main titles.
    - \ref org Description of how files have been organized
    - \ref mach3 How hw abstraction is implemented
    - \ref alg Alghorithm implemented and conceptual theory
    
    \page org Files Organization
    \details The project has to be organized in multiple abstraction Level to increase portability. 
    This kind of strategy means to separate the code in three Levels. \n
    - \b Level \b 0: Hardware/Driver Level: this level is strongly hardware related and regroup all the routines that works on registers/pins. 
    Registers operations must be transformed in macros, all placed in few files. Routines that work with this macros are all placed in the 
    Level 0 and are designed to return values for the Level 1 or 2. \n
    - \b Level \b 1: Firmware Level: Here are gathered all the algorithms that allow the correct work of the final system. 
    This functions are designed to receive an high level set point and, after some kind of elaboration, they have to return values that 
    can be useful for other Level 1-2 functions or that drives Level 0 functions. \n
    - \b Level \b 2: Software Level: this level is no hardware related at all. Here there have to be placed the state machine 
    that rules the motor behaviour. This level have to operate independently from other levels and it will control all other level with flags or values. \n
    In each level the programmer have to create some folders that group c/h - files according to functional competence (InputManagement, Interrupts, 
    Communication, etc.). \n
    This phylosophy has been implemented splitting routines in files, grouped in level folders.\n A partial example of how it's implemented this
    pyramidal structure is in the graph below.    

    \dot     digraph Implementation {
    node [shape=box, style=filled, fontname=Arial, fontsize=10];  
    rankdir=LR;compound=true; subgraph cluster1{ subgraph cluster11{
    main[shape=record,label="{<a> main.c | {<i>Initialization | <sm> HL State Machine |<ec>Error Check |<em>Error Manage}}"];
    label="Level 2\n Software Level";fontsize=8; style=filled; color=deepskyblue; } subgraph cluster12{
    MTC[shape=record,label="{<a>MCN1_mtc.c | {<m>Motion Control Routines}}"];
    ACM[shape=record,label="{<a>MCN1_acmotor.c | {HL Motor Control Routines|Loops Initialization|<lc> Loop Comutation}}"];
    Vbus[shape=record,label="{<a> VBA1_Vbatt.c | {Vbus Reading|<c>VbusCheck}}"];
    SOA[shape=record,label="{<a> SOA1_SOA.c | {SOA Check | Correction Computation}}"];
    Diag[shape=record,label="{<a> DIA1_Diagnostic.c | <m> Diag Managing }"];
    Gen[shape=record,label="{<a>XXX.c | Other Features... }"]
    label="Level 1\n Firmware Level";fontsize=8; style=filled; color=mediumpurple;
    }  subgraph cluster13{
    ISR[shape=record,label="{<a>TachoIsr.c | {IC Tacho Interrupt}}"];
    PWMISR[shape=record,label="{<a>PWMIsr.c | {PWM Interrupt}}"];
    EMER[shape=record,label="{<a>Emergency.c | {Disable Bridge|Set Stop Mode}}"];
    Gen2[shape=record,label="{XXX.c | Other Features... }"]
    label="Level 0\n Hardware Level";fontsize=8; style=filled; color=orange;
    }  label="Code-files (.c)";fontsize=10;
    }  subgraph cluster2{ subgraph cluster21{
    const[shape=record,label="{{<d>SPAL_def.h |SPAL_Setup.h} | Constant and \n definition}"];
    Table[shape=record,label="{{MOD2_DPWM.h|MOD2_Sine3Th.h} |<a> PWM \n Tables}"];
    h2[shape=record,label="{main.h | <a>Header File}"];
    label="Level 2\n Software Level";fontsize=8; style=filled; color=deepskyblue;
    } subgraph cluster22{ h1[shape=record,label="{{<1>MCN1_mtc.h|<2>MCN1_acmotor.h|<3>SOA1_SOA.h|<4>VBA1_Vbatt.h|<5>XXX.h} |<a> Header \n Files}"];
    label="Level 1\n Firmware Level";fontsize=8; style=filled; color=mediumpurple;
    } subgraph cluster23{ h0[shape=record,label="{<a>XXX.h |<h> Header Files}"];
    Pin[shape=record,label="{{<1>MAC0_Register.h} | MACROS \n HW relied}"]
    label="Level 0\n Hardware Level";fontsize=8; style=filled; color=orange;
    } const:a->h1:1;const:a->h1:2;const:a->h1:3;const:a->h1:4;const:a->h1:5;
    const:a->h0:a;const:a->Pin:1;const:a->Pin:2;
    h2:a->h1:a[lhead=cluster22,ltail=cluster21];h1:a->h0:a[lhead=cluster23,ltail=cluster22];
    label="Header-files (.h)";fontsize=10; }
    main:"sm"->ACM:"a";main:"sm"->MTC:"a"; main:"ec"->SOA:"a";main:"ec"->Vbus:"a"; main:"em"->Diag:"a";main:"sm"->Gen:"a";main:"i"->Gen:"a";
    MTC:m->PWMISR:a;ACM:lc->PWMISR:a;ACM:lc->ISR:a; Diag:m->EMER:a;
    h2:a->main:a[lhead=cluster11]; h1:a->Diag:m[lhead=cluster12]; h0:h->Gen2[lhead=cluster13];
    }   \enddot
    

    This graph shows how the level 2 can drive the levels below only passing variables and flags; as well the level 1 drives the level 0.
    Comunication between levels is bidirectional and it allows abstraction sacrificing code efficiency. \n

    \section finstr Final Structure
    The final structure implemented for c-files is the following:\n
    <b>Level 2: </b>
    - \ref main.c 
    
    <b>Level 1: </b>\n
    <em>Communication: </em>
    - \ref COM1_Com.c  
    
    <em>MotorControl: </em>
    - \ref MCN1_acmotor.c \n
    - \ref MCN1_mtc.c \n
    - \ref MCN1_Tacho.c  
    
    <em>Fir: </em>
    - \ref FIR1_Fir.c  

    <em>Input: </em>
    - \ref INP1_InputPWM.c  
    - \ref INP1_Key.c  

    <em>Timing: </em>
    - \ref TIM1_RTI.c  

    <em>Temperature: </em>
    - \ref TMP1_Temperature.c 

    <em>Vbatt: </em>
    - \ref VBA1_Vbatt.c  

    <em>Current: </em>
    - \ref CUR1_Current.c  
    
    <em>SOA: </em>
    - \ref SOA1_SOA.c 
    
    <em>Diagnostic: </em>
    - \ref DIA1_Diagnostic.c              

    <b>Level 0: </b>\n
    <em>Communication: </em>
    - \ref SCI.c 

    <em>GD: </em>  (Gate Driver)
    - \ref TLE7184F.c      

    <em>ADC: </em>
    - \ref ADM0_ADCS08.c  
    - \ref ADM0_ADCS12.c  

    <em>Interrupt: </em>
    - \ref INT0_PWMIsrS08.c \n
    - \ref INT0_PWMIsrS12.c \n
    - \ref INT0_IsrTableS08.c \n
    - \ref INT0_IsrTableS12.c \n
    - \ref INT0_TachoIsrS08.c \n
    - \ref INT0_TachoIsrS12.c \n
    - \ref INT0_DIAGIsrS08.c  
    - \ref INT0_DIAGIsrS12.c  

    <em>Initialization: </em>
    - \ref INI0_InitS08.c  
    - \ref INI0_InitS12.c  

    <em>Emergency: </em>
    - \ref EMR0_Emergency.c  
 
    The structure for header file is more or less the same because for each c-file exists an header file; the few differences consists in 
    some files added to increase HW abstraction. The final file list is the following:\n
    <b>Level 2: </b> \n
    <em>Constants: </em>
    - \ref main.h \n
    - \ref SPAL_def.h : Added \n
    - \ref SPAL_Setup.h : Added 
    
    <em>Modulation: </em>
    - \ref MOD2_Sine3Th.h : Added \n
    - \ref MOD2_DPWM.h : Added 
    
    <b>Level 1: </b>\n
    <em>Communication: </em>
    - \ref COM1_COM.h  
    
    <em>MotorControl: </em>
    - \ref MCN1_acmotor.h \n
    - \ref MCN1_mtc.h \n
    - \ref MCN1_Tacho.h  
    
    <em>Fir: </em>
    - \ref FIR1_FIR.h  

    <em>Input: </em>
    - \ref INP1_InputPWM.h  

    <em>Timing: </em>
    - \ref TIM1_RTI.h  

    <em>Temperature: </em>
    - \ref TMP1_Temperature.h 

    <em>Vbatt: </em>
    - \ref VBA1_Vbatt.h  

    <em>Current: </em>
    - \ref CUR1_Current.h  
    
    <em>SOA: </em>
    - \ref SOA1_SOA.h 
    
    <em>Diagnostic: </em>
    - \ref DIA1_Diagnostic.h              

    <b>Level 0: </b>\n
    <em>Communication: </em>
    - \ref SCI.h 

    <em>GD: </em>  (Gate Driver)
    - \ref TLE7184F.h     

    <em>ADC: </em>
    - \ref ADM0_ADCS08.c  
    - \ref ADM0_ADCS12.c  

    <em>Interrupt: </em>
    - \ref INT0_PWMIsrS08.h \n
    - \ref INT0_PWMIsrS12.h \n
    - \ref INT0_TachoIsrS08.h \n
    - \ref INT0_TachoIsrS12.h \n
    - \ref INT0_DIAGIsrS08.h  
    - \ref INT0_DIAGIsrS12.h  

    <em>Initialization: </em>
    - \ref INI0_InitS08.h  
    - \ref INI0_InitS12.h  
    
    <em>Macro: </em>
    - \ref MAC0_RegisterS08.h : Added 
    - \ref MAC0_RegisterS12.h : Added 

    <em>Emergency: </em>
    - \ref EMR0_Emergency.h  

    So the final file structure can be summarized in the following image, for code files:
\dot digraph Implementation {
node [shape=box, style=filled, fontname=Arial, fontsize=10];   compound=true; 
subgraph cluster1{ subgraph cluster11{
main[shape=record,label="main.c"];
label="Level 2\n Software Level";fontsize=8; style=filled; color=deepskyblue; 
} 
subgraph cluster12{
Gen[shape=record,label="{Com.c|MCN1_acmotor.c|MCN1_mtc.c|MCN1_Tacho.c|FIR1_Fir.c|INP1_InputPWM.c|INP1_Key.c|TIM1_RTI.c|TMP1_Temperature.c|VBA1_Vbatt.c|CUR1_Current.c|SOA1_SOA.c|DIA1_Diagnostic.c}"]
label="Level 1\n Firmware Level";fontsize=8; style=filled; color=mediumpurple;
}  
subgraph cluster13{
Gen2[shape=record,label="{SCI.c|TLE7184F.c|ADM0_ADCS08.c|ADM0_ADCS12.c|INT0_PWMIsrS08.c|INT0_PWMIsrS12.c|INT0_IsrTableS08.c|INT0_IsrTableS12.c|INT0_TachoIsrS08.c|INT0_TachoIsrS12.c|INT0_DIAGIsrS08.c|INT0_DIAGIsrS12.c|INI0_InitS08.c|INI0_InitS12.c|EMR0_Emergency.c}"]
label="Level 0\n Hardware Level";fontsize=8; style=filled; color=orange;
}  
label="Code-files (.c)";fontsize=10;
}
} \enddot

And for header files
\dot digraph Implementation { 
node [shape=box, style=filled, fontname=Arial, fontsize=10]; compound=true; 
subgraph cluster1{ subgraph cluster11{
	main[shape=record,label="{SPAL_def.h|SPAL_Setup.h|MOD2_Sine3Th.h|MOD2_DPWM.h}"];
	label="Level 2\n Software Level";fontsize=8; style=filled; color=deepskyblue; 
} 
subgraph cluster12{
	Gen[shape=record,label="{COM1_COM.h|MCN1_acmotor.h|MCN1_mtc.h|MCN1_Tacho.h|FIR1_FIR.h|INP1_InputPWM.h|TIM1_RTI.h|TMP1_Temperature.h|VBA1_Vbatt.h|CUR1_Current.h|SOA1_SOA.h|DIA1_Diagnostic.h}"]
	label="Level 1\n Firmware Level";fontsize=8; style=filled; color=mediumpurple;
}  
subgraph cluster13{
	Gen2[shape=record,label="{SCI.h|TLE7184F.h|ADM0_ADCS08.h|ADM0_ADCS12.h|INT0_PWMIsrS08.h|INT0_PWMIsrS12.h|INT0_TachoIsrS08.h|INT0_TachoIsrS128.h|INT0_DIAGIsrS08.h|INT0_DIAGIsrS12.h|INI0_InitS08.h|INI0_InitS12.h|MAC0_RegisterS08.h|MAC0_RegisterS12.h|EMR0_Emergency.h}"]
	label="Level 0\n Hardware Level";fontsize=8; style=filled; color=orange;
}  
label="Header-files (.h)";fontsize=10;
}
} \enddot

\page mach3 HW abstraction
\details To increase software portability and to maximize hardware independency, the software has to be writen respecting some rules:
- All the file strongly connected to hw management have to be isolated in Level zero files. This is done to limit changes, in place and 
size, that have to be done to change micro or pins mapping. \n
- Every atomic istruction, that modify some register or directly drive some hardware devices, has to be written with a MACRO that will be stored in few files.
As consequence, the whole code will be free from hardware or micro references and can be ported to another micro without great modifications. The only changes required
should be in the MACRO files. In this project the designed files to contain micro devices references are:\ref MAC0_RegisterS08.h \ref MAC0_RegisterS12.h .\n
- As for hardware references, the same things has to be done for defines, coefficients and constants. Also to be compliant with rules for good code writing, 
every number that has to be write in code has to be mapped in few files. This give the possibility to normalize phisics measures and to adapt internal micro coefficients
to real measures. The two files designed to store this type of macro are \ref SPAL_def.h and \ref SPAL_Setup.h

\page alg Main Alghorithm
\details The main alghoritm is based essentially on the V/f motor control. To achieve high efficiency and good stability the control present some
closed loops to create the needed feedback. Conceptual block diagram becomes the following:

\image rtf SBL500_De-Phi_Fw6_4_2rc4_De-PhiAlgorithmDiagram.png "Conceptual block diagram of the V/f Alghoritm" width=10cm   
\image html SBL500_De-Phi_Fw6_4_2rc4_De-PhiAlgorithmDiagram.png "Conceptual block diagram of the V/f Alghoritm" width=10cm   

In this scheme it's possible to see the control loops strictly relied to motor control; these are:
- \ref sKP Proportional \b Frequency loop to keep speed flat.
- \ref sKI Integral \b Voltage loop used to keep current phased with Back-EMF
- \ref sWM used to avoid motor current injecton on bus due to negative load torque on shaft

There are also two additional loops implemented to increase system protection that are not shown in the previous image:
- \ref sCSOA used to keep under control the bus current
- \ref sTSOA used to keep under control the system temperature

\section sKP Proportional Loop
\details This loop is implemented to suppress the 5Hz oscillations on rotor. This is done using as input the peak current that present the same oscillations 
that has the speed and so the bus current.
\image rtf ManagePhiProportionalLoop.png "Integral Correction Value" width=1cm
\image html ManagePhiProportionalLoophtml.png "Integral Correction Value" width=1cm
As it's shown in the picture above, the input PWM give the theoretical speed set point that the rotor have to achieve. After the conversion it has to be limited 
if the Battery voltage is below the motor Nominal voltage.\n In order to increase control stability the set point slope is limited to a maximum value; this coefficient is a 
electrical frequency function in order to have a low slope value at low speeds and a great value at high speeds.\n
In parallel it's calculated the difference between the requested phi value, computed using the peak current, and the real phi measured, given by the device connected on the W phase.
This error is multiplied to the Kp coefficient and is able to dump 5 Hz oscillations. \n
The starting requirement, with coefficients implementation, for this loop is in: \n
<a href="file://///Galileo/elettronica/AUTOMOTIVE_SPICE/ENG_Engineering_Process_Group/ENG.3_System_Architectural_Design/Pietro_kp_ki_kiwmill_23_04_2010.pdf"> kp_ki_kiwmill_23_04_2010.pdf </a>

\section sKI Integral Loop
\details The Integral loop is implemented to keep phased the current with the Back-EMF. The Integral Loop has be choosen, discarding proportion options, to limits 
the system bandwith and, at the same time, to have a steady state error equal to zero.The low bandwith it's necessary to avoid abroupt corrections that could keep to instability.
\image rtf ManagePhiIntegralLoop.png "Integral Correction Value" width=1cm
\image html ManagePhiIntegralLoophtml.png "Integral Correction Value" width=1cm
As shown in the figure above, there is a feedforward imposed by the V7f alghoritm, and a feedback that is implemented in a current loop 
that modify the voltage setpoint using as input the delta phi measured. The main V/f alghoritm is tuned to oversize the voltage associated to each frequency; 
this is done because during accellerations the laod torque, and so needed current, is greater than in steady state operation mode.
\image rtf loadcouple.gif "General mechanical torque on shaft"  width=1cm
\image html loadcouple.gif "General mechanical torque on shaft" width=1cm
As shown in formula, this overvoltage compensates the inertial couple that is needed during accelerations but disappear when the rotor speed is flat.
So, in steady state, to keep phased current and Back-EMF, the control present the feedback loop that decrease feedforward voltage with integrals dynamics
untill the measured phi is equal to the desired phi requested by peak current.\n
The starting requirement, with coefficients implementation, for this loop is in: \n
<a href="file://///Galileo/elettronica/AUTOMOTIVE_SPICE/ENG_Engineering_Process_Group/ENG.3_System_Architectural_Design/Pietro_kp_ki_kiwmill_23_04_2010.pdf"> kp_ki_kiwmill_23_04_2010.pdf </a>

\section sWM Windmill Loop
\details The windmill loop has been introduced to avoid that a negative load torque is applied on shaft can force the brushless to work as a generator.

\image rtf BrushlessWMArea.PNG "Limits imposed on Speed vs Torque Graph to avoid Generator Mode" width=1cm   
\image html BrushlessWMArea.PNG "Limits imposed on Speed vs Torque Graph to avoid Generator Mode" width=1cm   

This condition is triggered when the current became negative due to a negative couple resultant on shaft.
To avoid this condition that can cause failure on the car electrical sistem, the V/f algoritm can not manage this generator condition but correct the frequency set point to escape
from this problem. If car motion and so air flow on fan reduce load torque under a designed value Ilim, the Wm loop compute a correction to increase the set point and so the current
requested on bus. 
\image rtf Windmillhtml.png "Windmill feedfoward block diagram"  width=1cm
\image html Windmillhtml.png "Windmill feedfoward block diagram" width=1cm

The implemented scheme is in the picture below.\n
The I limit is a function of speed as explained in this formula

\image rtf wmIlim.gif "I lim computation"  width=1cm
\image html wmIlim.gif "I lim computation" width=1cm

If the current go below zero the motor works as generator and this could be a problem. Because of low dynamic band imposed to reduce costs, the limits is placed to value greater than zero
to intercept windmill conditions before the theoretical limit.\n
The final correction added to frequency is so computed:

\image rtf wmInt.gif "Integral Correction Value" width=1cm
\image html wmInt.gif "Integral Correction Value" width=1cm

The starting requirement, with coefficients implementation, for this loop is in: \n
<a href="file://///Galileo/elettronica/AUTOMOTIVE_SPICE/ENG_Engineering_Process_Group/ENG.3_System_Architectural_Design/Pietro_kp_ki_kiwmill_23_04_2010.pdf"> kp_ki_kiwmill_23_04_2010.pdf </a>

\section sCSOA Current SOA (Safe Operationg Area)
\details This feedback is mandatory to avoid overcurrent on rotor phases. If the motor present an overcurrent during the start up, it's possible that
some kind of failure (mechanical or electrical) is occurred. This can be the rotor blocked, two phases short-circuited or something relied to an increase of
load torque; if this condition occurred during the start up, the rampup is immediately stopped. In particular this condition can detect a rotor block failure
without additionals sensors.\n
Otherwise if the start up has been correctly performed and a designed time is passed, when an overcurrent is detected, the SOA alghoritm computes a correction that
is applied and increased to reduce current consumption. This correction is computed and applied with an integral logic; if this feature can't erase the overcurrent
after a time of 1 minute, the V/f alghoritm is stopped and a new restart is scheduled.

\image rtf SOAcurrent.png "Conceptual Block Diagram of Current SOA Implementation" width=1cm
\image html SOAcurrenthtml.png "Conceptual Block Diagram of Current SOA Implementation" width=1cm 
           
\section sTSOA Temperature SOA (Safe Operationg Area)
\details The Temperature SOA performs an initial check to avoid starts in over/under temperature conditions. This is done to avoid mechanical failures due to 
material termical ranges.\n
Otherwise, if the motor is running, the system temperature is continuosly monitored. If temperature exceeds a certain value, control loop implements a derating 
to reduce power consumption and so Joule losses. The output value is subtracted from input set point and so to the speed inposed on rotor.

\image rtf SOATemp.png "Conceptual Block Diagram of Temperature SOA Implementation" width=1cm
\image html SOATemphtml.png "Conceptual Block Diagram of Temperature SOA Implementation" width=1cm 
 
\section sGSCH Detailed Block Diagram
\details The detailed block diagram that has been described is shown in the picture below. \n
\image rtf SBL300_De-Phi_Fw5.4.1rc1_core_4.PNG "Detailed block diagram of the V/f Alghoritm" width=10cm   
\image html SBL300_De-Phi_Fw5.4.1rc1_core_4.PNG "Detailed block diagram of the V/f Alghoritm" width=10cm   


    
*/

#ifndef MAIN_H
#define MAIN_H

#include "SpalTypes.h"

/* Public Constants -------------------------------------------------------- */
/**
\def NO_ERROR
    \brief Error reset
    \details Constant used to crush Error flag variable to zero
\def RPI_PROTECTION_FLAG
    \brief Tacho overflow number Error flag bit number
    \details This constant identifies the bit number that is set when the tacho overflow number is too high. This means a suddenly rotor stop
    or a failure in the sensing chain. The difference from the \ref W_PROTECTION_FLAG is that this check is executed in main loop and is independent 
    from zero crossing Tacho events
\def RPI_PROTECTION
    \brief Tacho overflow number Error flag mask
    \details This constant creates a mask for bit number that is set when the tacho overflow number is too high. This means a suddenly rotor stop
    or a failure in the sensing chain. The difference from the \ref W_PROTECTION_FLAG is that this check is executed in main loop and is independent 
    from zero crossing Tacho events
\def V_BUS_FLAG
    \brief Battery voltage Error flag bit number
    \details This error is set whenever a failure or an undervoltage/overvoltage is detected in the Vbus sensing chain.
\def V_BUS
    \brief Battery voltage Error flag mask
    \details This error is set whenever a failure or an undervoltage/overvoltage is detected in the Vbus sensing chain.              
\def T_AMB_FLAG
    \brief Ambient Temperature sensing Error flag bit number
    \details This error is set when an error is detected checking the temperature from the external sensor         
\def T_AMB
    \brief Ambient Temperature sensing Error flag mask
    \details  This error is set when an error is detected checking the temperature from the external sensor             
\def WINDMILL_FLAG
    \brief Windmill Error flag bit number
    \details This flag identify a problem in the windmill compensation       
\def WINDMILL
    \brief Windmill Error flag mask
    \details This flag identify a problem in the windmill compensation           
\def I_LIM_FLAG
    \brief Current SOA Error flag bit number
    \details This flag is set when a SOA overcoming is detected.         
\def I_LIM    
    \brief Current SOA Error flag mask
    \details This flag is set when a SOA overcoming is detected.              
\def INVALID_PWM_IN_FLAG
    \brief Input Set Point Error flag bit number
    \details If the Input set point disappear or have invalid ranges, this flag is set.
\def INVALID_PWM_IN     
    \brief Input Set Point Error flag mask
    \details If the Input set point disappear or have invalid ranges, this flag is set.      
\def GD_ERROR_FLAG
    \brief Gate Driver Error flag bit number
    \details The gate driver has recognized a failure and it is in protection mode.      
\def GD_ERROR 
    \brief Gate Driver Error flag mask
    \details The gate driver has recognized a failure and it is in protection mode.           

\def RESTART_MASK (u8)(RPI_PROTECTION + I_LIM + GD_ERROR)
    \brief  Error Mask for multiple error
    \details Error mask that regroup all errors that have to be managed a limited number of time
    with additional diagnostic signal if required.
*/

#define NO_ERROR            (u16)0 
#define W_PROTECTION_FLAG   (u16)0
#define W_PROTECTION        (u16)((u16)1 << W_PROTECTION_FLAG)
#define RPI_PROTECTION_FLAG (u16)1
#define RPI_PROTECTION      (u16)((u16)1 << RPI_PROTECTION_FLAG)
#define START_ERROR_FLAG    (u16)2
#define START_ERROR         (u16)((u16)1 << START_ERROR_FLAG)
#define V_BUS_FLAG          (u16)3 
#define V_BUS               (u16)((u16)1 << V_BUS_FLAG)
#define T_AMB_FLAG          (u16)4 
#define T_AMB               (u16)((u16)1 << T_AMB_FLAG)
#define WINDMILL_FLAG       (u16)5 
#define WINDMILL            (u16)((u16)1 << WINDMILL_FLAG)
#define I_LIM_FLAG          (u16)6
#define I_LIM               (u16)((u16)1 << I_LIM_FLAG)
#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
  #define PARTIAL_STALL_FLAG  (u16)7
  #define PARTIAL_STALL       (u16)((u16)1 << PARTIAL_STALL_FLAG)
#endif  /* DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE */
#define INVALID_PWM_IN_FLAG (u16)8
#define INVALID_PWM_IN      (u16)((u16)1 << INVALID_PWM_IN_FLAG)
#define KEY_INPUT_OFF_FLAG  (u16)9
#define KEY_INPUT_OFF       (u16)((u16)1 << KEY_INPUT_OFF_FLAG)
#define GD_ERROR_FLAG       (u16)10
#define GD_ERROR            (u16)((u16)1 << GD_ERROR_FLAG)
#ifdef RVS 
  #define RVS_STOP_FLAG       (u16)11
  #define RVS_STOP            (u16)((u16)1 << RVS_STOP_FLAG)
#endif  /* RVS */
#define ROF_FAIL_FLAG      (u16)12
#define ROF_FAIL           (u16)((u16)1 << ROF_FAIL_FLAG)
#define FAULT_ERROR_FLAG   (u16)13
#define FAULT_ERROR        (u16)((u16)1 << FAULT_ERROR_FLAG)
#define LOCKED_ROTOR_FLAG  (u16)14
#define LOCKED_ROTOR       (u16)((u16)1 << LOCKED_ROTOR_FLAG)
#ifdef OVERCURRENT_ROF
  #define I_TRANSIENT_FLAG    (u16)15
  #define I_TRANSIENT         (u16)((u16)1 << I_TRANSIENT_FLAG)
#endif  /* OVERCURRENT_ROF */

#define RESTART_MASK (RPI_PROTECTION + I_LIM + GD_ERROR + FAULT_ERROR)

#define IDLE_TIME_WAIT_PWM_IN               (u16)62	  /* 500 ms at 8 ms timebase */
#ifdef KEY_SHORT_LOGIC
  #define CHECK_KEY_SHORT_TIME_WAIT         (u16)31	  /* 250 ms at 8 ms timebase */
#endif  /* KEY_SHORT_LOGIC */
#define IDLE_TIME                           (u16)62   /* 500 ms at 8 ms timebase */
#define RESTART_MASK_TIME                   (u16)625  /*   5  s at 8 ms timebase */
#define START_NUMBER_RESET_TIME             (u16)1250 /*  10  s at 8 ms timebase */
#define RESTART_MASK_MAX_VALUE              (u8)5

#ifdef LIN_INPUT
  /* Filtro sul feedback di velocita' */
  #define LIN_SPEED_PRESCALER_SHIFT                 1
  #define LIN_SPEED_PRESCALER_FACT                  (u8)(1<<LIN_SPEED_PRESCALER_SHIFT)
  #define LIN_SPEED_SCALER_BIT                      5
  #define LIN_SPEED_SCALER_FACT                     (u8)(1<<LIN_SPEED_SCALER_BIT)  
  #define LIN_SPEED_INT_STEP_FREQ                   (u16)((u32)((u32)1000*LIN_SPEED_SCALER_FACT)/(u16)LIN_TIMEBASE_MSEC)    /* LIN_TIMEBASE_MSEC = 10 ms */
  #define LIN_SPEED_TIME_CONST                      0.2 /* RC = 0.2 s */
  #define LIN_SPEED_FREQ_CONST                      LIN_SPEED_SCALER_FACT/LIN_SPEED_TIME_CONST
  #define LIN_SPEED_HYST_RPM                        500 /* [rpm] */
  #define LIN_SPEED_HYST_FREQ                       (float32_t)((float32_t)((float32_t)LIN_SPEED_HYST_RPM*POLE_PAIR_NUM)/60)    
  #define LIN_SPEED_HYST_PU                         (s16)((float32_t)((float32_t)LIN_SPEED_HYST_FREQ*FREQUENCY_RES_FOR_PU_BIT)/BASE_FREQUENCY_HZ)

  #define BEMF_SPEED_TOL_NUM_WM                     (u8)64     /* 64/(1<<7) = 64/128 <=> 50% - Tolleranza sulla lettura di bemf, con drive in sotto Wind-mill.                 */
  #define BEMF_SPEED_TOL_NUM                        (u8)32     /* 32/(1<<7) = 32/128 <=> 25% - Tolleranza sulla lettura di bemf, con drive in evoluzione libera.               */
                                                               /* IanniF: rivedere valore minimo ammissibile per BEMF_SPEED_TOL_NUM, onde non avere spikes sotto sovrimposte!! */
  #define BEMF_SPEED_TOL_SHIFT                      (u8)7
  #define TACHO_OVF_FOR_SPEED_RESET                 (u8)50     /* Valore massimo di overflow del tacho per il reset del feedback di velocita'. */
  
  #define RPM_MIN_SPEED_FEEDBACK                    (u16)120   /* Velocita' minima per feedback di velocita' via LIN. */
  #define EL_FREQ_MIN_SPEED_FEEDBACK                (float32_t)((float32_t)((float32_t)RPM_MIN_SPEED_FEEDBACK*POLE_PAIR_NUM)/60)
  #define EL_FREQ_MIN_SPEED_FEEDBACK_PU             (float32_t)((float32_t)EL_FREQ_MIN_SPEED_FEEDBACK/BASE_FREQUENCY_HZ)
  #define EL_FREQ_MIN_SPEED_FEEDBACK_PU_RES_BIT     (u16)((float32_t)((float32_t)EL_FREQ_MIN_SPEED_FEEDBACK*FREQUENCY_RES_FOR_PU_BIT)/BASE_FREQUENCY_HZ)  
  
  #define BEMF_MIN_COUNT_MAX                        5          /* Numero di volte iin cui la bemf rilevata e' sotto la soglia minima, prima di avere velocita' segnalata nulla. */
#endif    /* LIN_INPUT */

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
  #define DELTA_RPM_PARTIAL_STALL               50  /* [rpm] */
  #define DELTA_FREQ_PARTIAL_STALL              (float32_t)((float32_t)((float32_t)DELTA_RPM_PARTIAL_STALL*(float32_t)POLE_PAIR_NUM)/(float32_t)60)
  #define DELTA_FREQ_PARTIAL_STALL_PU           (float32_t)((float32_t)DELTA_FREQ_PARTIAL_STALL/BASE_FREQUENCY_HZ)
  #define DELTA_FREQ_PARTIAL_STALL_PU_RES_BIT   (u16)((float32_t)((float32_t)DELTA_FREQ_PARTIAL_STALL*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)BASE_FREQUENCY_HZ)  
    
  #define DELTA_LIM_PARTIAL_STALL_PU_RES_BIT    (u32)((u32)DELTA_FREQ_PARTIAL_STALL_PU_RES_BIT<<DELTA_REG_COMPUTATION_RES_SHIFT)
  #define MAX_IPK_LIM_PARTIAL_STALL_PU_RES_BIT  (u32)((u32)DELTA_FREQ_PARTIAL_STALL_PU_RES_BIT<<MAX_CURR_REG_COMPUTATION_RES_SHIFT)  
#endif  /* DIAGNOSTIC_STATE */

#ifdef KEY_SHORT_LOGIC
  #define TIME_TO_REFRESH_KEY_SHORT_SEC         300 /* [s] <=> 5 min */
  #define TIME_TO_REFRESH_KEY_SHORT             (u16)((u32)((u32)TIME_TO_REFRESH_KEY_SHORT_SEC*(u32)1000)/(u32)8)
#endif  /* KEY_SHORT_LOGIC */

/* Public type definition -------------------------------------------------- */
/** 
    \typedef SystStatus_t
    \brief Machine States
    \details This enumerator define names of machine states.
*/

typedef enum
{
    IDLE,       /* !< Waiting for Initial starting conditions        */
    PARK,       /* !< Parking                                        */
    RUN_ON_FLY, /* !< Transition State that allow Run_On_Fly looking */
    RUN,        /* !< Running in normal condition                    */
    WAIT,       /* !< Waiting for next start up                      */
    FAULT       /* !< Fault state for errors or invalid parameters   */
} SystStatus_t;

#if ( defined(DEBUG_MODE) || defined(ENABLE_RS232_DEBUG) )
  typedef struct  {
    u16 u16DebugVar1;
    u32 u32DebugVar2;
    s16 s16DebugVar3;
    s32 s32DebugVar4;
  } structDebug;
#endif  /* defined(DEBUG_MODE) || defined(ENABLE_RS232_DEBUG) */

/* Public Variables -------------------------------------------------------- */

#if ( defined(DEBUG_MODE) || defined(ENABLE_RS232_DEBUG) )
  extern structDebug strDebug;
#endif  /* defined(DEBUG_MODE) || defined(ENABLE_RS232_DEBUG) */

/* Public Functions -------------------------------------------------------- */

void Main_SetBit_u16ErrorType(u16 errTyp);
void Main_ClrBit_u16ErrorType(u16 errTyp);
#if (defined(ENABLE_RS232_DEBUG) || defined(BMW_LIN_ENCODING))
u16  Main_Get_u16ErrorType(void);
#endif

u8   Main_Get_u8StartNumber(void);
#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
  u16  Main_Get_u16ErrorTypeMem(void);
#endif  /* DIAGNOSTIC_STATE */

#endif /* MAIN_H */

/*** (c) 2007 SPAL Automotive ****************** END OF FILE **************/

