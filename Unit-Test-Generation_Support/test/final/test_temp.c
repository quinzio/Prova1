/*****************************************************************************/
/*                            Cantata Test Script                            */
/*****************************************************************************/
/*
 *    Filename: test_MCN1_acmotor.c
 *    Author: MUNARID
 *    Generated on: 05-Mar-2020 12:59:21
 *    Generated from: MCN1_acmotor.c
 */
/*****************************************************************************/
/* Environment Definition                                                    */
/*****************************************************************************/

#define TEST_SCRIPT_GENERATOR 2

/* Include files from software under test */
#include "../../../../Sources/cantata.h"
#include "../../../../Sources/SPAL_Setup.h"
#include "../../../../Sources/SpalTypes.h"
#include "../../../../Sources/SpalBaseSystem.h"
#include <MC9S12GN32.h>
#include "../../../../Sources/SPAL_def.h"
#include "../../../../Sources/MAC0_RegisterS12.h"
#include "../../../../Sources/ADM0_ADCS12.h"
#include "../../../../Sources/main.h"
#include "../../../../Sources/MCN1_acmotor.h"
#include "../../../../Sources/MCN1_mtc.h"
#include "../../../../Sources/MCN1_RegPI.h"
#include "../../../../Sources/FIR1_FIR.h"
#include "../../../../Sources/TIM1_RTI.h"
#include "../../../../Sources/MCN1_Tacho.h"
#include "../../../../Sources/TLE7184F.h"
#include "../../../../Sources/VBA1_Vbatt.h"
#include "../../../../Sources/CUR1_Current.h"
#include "../../../../Sources/EMR0_Emergency.h"
#include "../../../../Sources/TMP1_Temperature.h"
#include "../../../../Sources/INP1_InputSP.h"
#include "../../../../Sources/WTD1_Watchdog.h"
#include "../../../../Sources/POWER1_MaxPower.h"
#include "../../../../Sources/PBATT1_Power.h"
#include "../../../../Sources/INT0_PWMIsrS12.h"
//#include "../../../../Sources/MCN1_SINPU.h"
#include "../../../../Sources/RVP1_RVP.h"
#include "../../../../Sources/DIA1_Diagnostic.h"
#include "../../../../Sources/SOA1_SOA.h"
#include "../../../../Sources/INT0_RTIIsrS12.h"

#define CANTATA_DEFAULT_VALUE 0 /* Default value of variables & stub returns */

#include <cantpp.h>  /* Cantata Directives */
/* pragma qas cantata testscript start */
/*****************************************************************************/
/* Global Data Definitions                                                   */
/*****************************************************************************/

/* Global Functions */
extern void ACM_Init();
extern void ACM_InitOnFly();
extern void ACM_ManageParking();
extern void ACM_InitControlStruct();
extern void ACM_UpdateControlStruct(u16 u16ActualBemf, u16 u16ActualFreq);
extern void ACM_DoMotorControl();
extern void ACM_DoFrequencyRampPU(u16 u16NewFreqPU);
extern u16 ACM_GetMaximumVoltageApp();
extern void ACM_SetVoltageVoverF();
extern u16 ACM_RsTempCorr();
extern u16 ACM_ComputeDeltaMax();
extern void ACM_ComputeDeltaAngle();
extern void ACM_WindMillLoop();
extern void ACM_DeltaLimiterLoop();
extern void ACM_MaxIpkLoop();
extern void ACM_MaxPbattLoop();
extern void ACM_TODLoop();
extern void ACM_PhaseVoltageLoop();
extern BOOL ACM_CheckWindmillRecovery();
extern u16 SetFrequencySetPoint(u8 u8InRef);
extern u16 ACM_SetFrequencySetPointFixed(u8 u8InRef);
extern void ACM_Set_u16FreqRampPU(u16 value);
extern u16 ACM_Get_u16FreqRampPU();
extern void ACM_Set_u16FreqRampSmoothPU(u16 value);
extern u16 ACM_Get_u16FreqRampSmoothPU();
extern void ACM_ManagePulseCanc();
void MTC_ResetZCECntrl();
void MTC_InitMTCVariables();
void Tacho_InitTachoVariables();
void MTC_InitBatteryDegragation();
void Power_InitPowerStruct();
void REG_SetParPI(PIenum enumReg, u16 u16IntGain, u16 u16PropGain,
		s32 s32RegLimLow, s32 s32RegLimHigh, BOOL BOOLFreeze, u16 u16IntStep);
void REG_InitVarPI(PIenum enumReg, s16 s16RegInput, s16 s16RegFeedback,
		s32 s32Integral, s32 s32Proportional, s32 s32RegOutput);
void FIR_SetParFilt(FiltEnum enumFilt, u16 u16SampleFreq, u16 u16PoleFreq,
		u16 u16ZeroFreq, s16 s16Hyst, u8 u8FiltType);
void FIR_InitVarFilt(FiltEnum enumFilt, s16 s16OldInput, s16 s16NewOutput);
void RVP_SetRVPOn();
void TLE_EnableOutputs();
void INT0_PWM_FailSafeMonitor();
void WTD_ResetWatchdog();
void INT0_PWM_FailSafeMonitor_rst();
OperatingMode_t InputSP_GetOperatingMode();
void RTI_SetMainTimeBase(u8 u8Period);
BOOL RTI_IsMainTimeElapsed();
void INT0_FailSafeMonitor();
void DIAG_ManageDiagnosticToECU(SystStatus_t enLocState, u16 u16DiagErrorType);
void INT0_FailSafeMonitor_rst();
void MTC_SettingForParkingPhase();
void Tacho_ResetTachoOverflowNumber();
BOOL MTC_ParkAngleSweep();
u16 MTC_ComputeParkingTime();
BOOL RTI_IsRegPeriodElapsed();
u8 InputSP_GetInputSetPoint();
void MaxPower_PowerSetPoint();
void Tacho_ManageTachoCapture();
void Power_UpdatePowerStruct();
BOOL MTC_UpdateSine(u16 u16NewVoltage, u16 u16NewFrequency);
BOOL Temp_Get_BOOLSOATemperatureCorr();
void FIR_UpdateFilt(FiltEnum enumFilt, s16 s16NewInput);
s16 INT0_Get_strTempDrive_s16TempAmbReadPU();
u16 Curr_GetPeakCurrent();
void REG_UpdateInputPI(PIenum enumReg, s16 s16RegSp, s16 s16RegSample);
void REG_UpdateRegPI(PIenum enumReg);
void Main_SetBit_u16ErrorType(u16 errTyp);
void Main_ClrBit_u16ErrorType(u16 errTyp);
u32 MaxPower_GetPowerSetPoint();
u32 Power_Get_structPowerMeas_u32PbattPU();
u16 Curr_GetCurrent();
u16 MTC_GetScaledVoltage();
void EMR_EmergencyDisablePowerStage();

/* Global data */
C_D_ISR_Status_t C_D_ISR_Status;
extern s16 s16PmaxDeltaFreq;
extern BOOL BOOLSteadyStateFlag;
u16 u16HwRefTimer;
BOOL BOOLHwRefFlag;
u16 u16UpdateLoopTime;
u16 u16UpdateLoopTimer;
PWMDTY01STR _PWMDTY01;
PWMDTY23STR _PWMDTY23;
PWMDTY45STR _PWMDTY45;
TC2STR _TC2;
TIESTR _TIE;
TC1STR _TC1;
extern PMSMstructPU strMachine;
extern u16 (*ACM_SetFrequencySetPoint)(u8);
structIbattPIreg IbattReg;
structIbattLogic IbattStateMachine;
structVarFilt VectVarFilt[VECT_FILT_LENGHT];
u16 buffu16ADC_READING[(u8) 8U];
extern const u16 SINPU[128];
structRegIO VectVarPI[VECT_PI_LENGTH];
u16 u16VbattAvg;
structPIreg VectParPI[VECT_PI_LENGTH];
extern u8 u8WMerrCounts;
extern rampStruct rampGenerator;
u8 u8MTCStatus;

/* Expected variables for global data */
C_D_ISR_Status_t expected_C_D_ISR_Status;
s16 expected_s16PmaxDeltaFreq;
BOOL expected_BOOLSteadyStateFlag;
u16 expected_u16HwRefTimer;
BOOL expected_BOOLHwRefFlag;
u16 expected_u16UpdateLoopTime;
u16 expected_u16UpdateLoopTimer;
PWMDTY01STR expected__PWMDTY01;
PWMDTY23STR expected__PWMDTY23;
PWMDTY45STR expected__PWMDTY45;
TC2STR expected__TC2;
TIESTR expected__TIE;
TC1STR expected__TC1;
PMSMstructPU expected_strMachine;
u16 (*expected_ACM_SetFrequencySetPoint)(u8);
structIbattPIreg expected_IbattReg;
structIbattLogic expected_IbattStateMachine;
structVarFilt expected_VectVarFilt[VECT_FILT_LENGHT];
u16 expected_buffu16ADC_READING[(u8) 8U];
structRegIO expected_VectVarPI[VECT_PI_LENGTH];
u16 expected_u16VbattAvg;
structPIreg expected_VectParPI[VECT_PI_LENGTH];
u8 expected_u8WMerrCounts;
rampStruct expected_rampGenerator;
u8 expected_u8MTCStatus;

/* This function initialises global data to default values. This function       */
/* is called by every test case so must not contain test case specific settings */
static void initialise_global_data() {
	TEST_SCRIPT_WARNING("Verify initialise_global_data()\n");
	INITIALISE(C_D_ISR_Status);
	INITIALISE(s16PmaxDeltaFreq);
	INITIALISE(BOOLSteadyStateFlag);
	INITIALISE(u16HwRefTimer);
	INITIALISE(BOOLHwRefFlag);
	INITIALISE(u16UpdateLoopTime);
	INITIALISE(u16UpdateLoopTimer);
	INITIALISE(_PWMDTY01);
	INITIALISE(_PWMDTY23);
	INITIALISE(_PWMDTY45);
	INITIALISE(_TC2);
	INITIALISE(_TIE);
	INITIALISE(_TC1);
	INITIALISE(strMachine);
	INITIALISE(ACM_SetFrequencySetPoint);
	INITIALISE(IbattReg);
	INITIALISE(IbattStateMachine);
	INITIALISE(VectVarFilt);
	INITIALISE(buffu16ADC_READING);
	/* No initialisation for const data: SINPU */
	INITIALISE(VectVarPI);
	INITIALISE(u16VbattAvg);
	INITIALISE(VectParPI);
	INITIALISE(u8WMerrCounts);
	INITIALISE(rampGenerator);
	INITIALISE(u8MTCStatus);
}

/* This function copies the global data settings into expected variables for */
/* use in check_global_data(). It is called by every test case so must not   */
/* contain test case specific settings.                                      */
static void initialise_expected_global_data() {
	TEST_SCRIPT_WARNING("Verify initialise_expected_global_data()\n");
	COPY_TO_EXPECTED(C_D_ISR_Status, expected_C_D_ISR_Status);
	COPY_TO_EXPECTED(s16PmaxDeltaFreq, expected_s16PmaxDeltaFreq);
	COPY_TO_EXPECTED(BOOLSteadyStateFlag, expected_BOOLSteadyStateFlag);
	COPY_TO_EXPECTED(u16HwRefTimer, expected_u16HwRefTimer);
	COPY_TO_EXPECTED(BOOLHwRefFlag, expected_BOOLHwRefFlag);
	COPY_TO_EXPECTED(u16UpdateLoopTime, expected_u16UpdateLoopTime);
	COPY_TO_EXPECTED(u16UpdateLoopTimer, expected_u16UpdateLoopTimer);
	COPY_TO_EXPECTED(_PWMDTY01, expected__PWMDTY01);
	COPY_TO_EXPECTED(_PWMDTY23, expected__PWMDTY23);
	COPY_TO_EXPECTED(_PWMDTY45, expected__PWMDTY45);
	COPY_TO_EXPECTED(_TC2, expected__TC2);
	COPY_TO_EXPECTED(_TIE, expected__TIE);
	COPY_TO_EXPECTED(_TC1, expected__TC1);
	COPY_TO_EXPECTED(strMachine, expected_strMachine);
	COPY_TO_EXPECTED(ACM_SetFrequencySetPoint,
			expected_ACM_SetFrequencySetPoint);
	COPY_TO_EXPECTED(IbattReg, expected_IbattReg);
	COPY_TO_EXPECTED(IbattStateMachine, expected_IbattStateMachine);
	COPY_TO_EXPECTED(VectVarFilt, expected_VectVarFilt);
	COPY_TO_EXPECTED(buffu16ADC_READING, expected_buffu16ADC_READING);
	COPY_TO_EXPECTED(VectVarPI, expected_VectVarPI);
	COPY_TO_EXPECTED(u16VbattAvg, expected_u16VbattAvg);
	COPY_TO_EXPECTED(VectParPI, expected_VectParPI);
	COPY_TO_EXPECTED(u8WMerrCounts, expected_u8WMerrCounts);
	COPY_TO_EXPECTED(rampGenerator, expected_rampGenerator);
	COPY_TO_EXPECTED(u8MTCStatus, expected_u8MTCStatus);
}

/* This function checks global data against the expected values. */
static void check_global_data() {
	TEST_SCRIPT_WARNING("Verify check_global_data()\n");
	CHECK_U_CHAR(C_D_ISR_Status, expected_C_D_ISR_Status);
	CHECK_S_INT(s16PmaxDeltaFreq, expected_s16PmaxDeltaFreq);
	CHECK_U_CHAR(BOOLSteadyStateFlag, expected_BOOLSteadyStateFlag);
	CHECK_U_INT(u16HwRefTimer, expected_u16HwRefTimer);
	CHECK_U_CHAR(BOOLHwRefFlag, expected_BOOLHwRefFlag);
	CHECK_U_INT(u16UpdateLoopTime, expected_u16UpdateLoopTime);
	CHECK_U_INT(u16UpdateLoopTimer, expected_u16UpdateLoopTimer);
	CHECK_MEMORY("_PWMDTY01", &_PWMDTY01, &expected__PWMDTY01,
			sizeof(expected__PWMDTY01));
	CHECK_MEMORY("_PWMDTY23", &_PWMDTY23, &expected__PWMDTY23,
			sizeof(expected__PWMDTY23));
	CHECK_MEMORY("_PWMDTY45", &_PWMDTY45, &expected__PWMDTY45,
			sizeof(expected__PWMDTY45));
	CHECK_MEMORY("_TC2", &_TC2, &expected__TC2, sizeof(expected__TC2));
	CHECK_MEMORY("_TIE", &_TIE, &expected__TIE, sizeof(expected__TIE));
	CHECK_MEMORY("_TC1", &_TC1, &expected__TC1, sizeof(expected__TC1));
	CHECK_MEMORY("strMachine", &strMachine, &expected_strMachine,
			sizeof(expected_strMachine));
	CHECK_ADDRESS(ACM_SetFrequencySetPoint, expected_ACM_SetFrequencySetPoint);
	CHECK_MEMORY("IbattReg", &IbattReg, &expected_IbattReg,
			sizeof(expected_IbattReg));
	CHECK_MEMORY("IbattStateMachine", &IbattStateMachine,
			&expected_IbattStateMachine, sizeof(expected_IbattStateMachine));
	CHECK_MEMORY("VectVarFilt", VectVarFilt, expected_VectVarFilt,
			sizeof(expected_VectVarFilt));
	CHECK_MEMORY("buffu16ADC_READING", buffu16ADC_READING,
			expected_buffu16ADC_READING, sizeof(expected_buffu16ADC_READING));
	CHECK_MEMORY("VectVarPI", VectVarPI, expected_VectVarPI,
			sizeof(expected_VectVarPI));
	CHECK_U_INT(u16VbattAvg, expected_u16VbattAvg);
	CHECK_MEMORY("VectParPI", VectParPI, expected_VectParPI,
			sizeof(expected_VectParPI));
	CHECK_U_CHAR(u8WMerrCounts, expected_u8WMerrCounts);
	CHECK_MEMORY("rampGenerator", &rampGenerator, &expected_rampGenerator,
			sizeof(expected_rampGenerator));
	CHECK_U_CHAR(u8MTCStatus, expected_u8MTCStatus);
}

/* Prototypes for test functions */
void run_tests();
void test_ACM_Init(int);
void test_ACM_InitOnFly(int);
void test_ACM_ManageParking(int);
void test_ACM_InitControlStruct(int);
void test_ACM_UpdateControlStruct(int);
void test_ACM_DoMotorControl(int);
void test_ACM_DoFrequencyRampPU(int);
void test_ACM_DoFrequencyRampPU_001(int);
void test_ACM_DoFrequencyRampPU_002(int);
void test_ACM_DoFrequencyRampPU_003(int);
void test_ACM_DoFrequencyRampPU_004(int);
void test_ACM_DoFrequencyRampPU_005(int);
void test_ACM_GetMaximumVoltageApp(int);
void test_ACM_SetVoltageVoverF(int);
void test_ACM_RsTempCorr(int);
void test_ACM_ComputeDeltaMax(int);
void test_ACM_ComputeDeltaAngle(int);
void test_ACM_WindMillLoop(int);
void test_ACM_DeltaLimiterLoop(int);
void test_ACM_MaxIpkLoop(int);
void test_ACM_MaxPbattLoop(int);
void test_ACM_TODLoop(int);
void test_ACM_PhaseVoltageLoop(int);
void test_ACM_CheckWindmillRecovery(int);
void test_ACM_CheckWindmillRecovery_001(int);
void test_ACM_CheckWindmillRecovery_002(int);
void test_SetFrequencySetPoint(int);
void test_ACM_SetFrequencySetPointFixed(int);
void test_ACM_Set_u16FreqRampPU(int);
void test_ACM_Get_u16FreqRampPU(int);
void test_ACM_Set_u16FreqRampSmoothPU(int);
void test_ACM_Get_u16FreqRampSmoothPU(int);
void test_ACM_ManagePulseCanc(int);

/*****************************************************************************/
/* Coverage Analysis                                                         */
/*****************************************************************************/
/* Coverage Rule Set: 100% Entry Point + Statement + Call + Decision Coverage */
static void rule_set(char* cppca_sut, char* cppca_context) {
#ifdef CANTPP_SUBSET_DEFERRED_ANALYSIS
	TEST_SCRIPT_WARNING("Coverage Rule Set ignored in deferred analysis mode\n");
#elif CANTPP_INSTRUMENTATION_DISABLED
	TEST_SCRIPT_WARNING("Instrumentation has been disabled\n");
#else
	ANALYSIS_CHECK("100% Entry Point Coverage", cppca_entrypoint_cov, 100.0);

	ANALYSIS_CHECK("100% Statement Coverage", cppca_statement_cov, 100.0);

	ANALYSIS_CHECK("100% Call Return Coverage", cppca_callreturn_cov, 100.0);

	ANALYSIS_CHECK("100% Decision Coverage", cppca_decision_cov, 100.0);

	REPORT_COVERAGE(
			cppca_entrypoint_cov | cppca_statement_cov | cppca_callreturn_cov
					| cppca_decision_cov, cppca_sut,
			cppca_all_details | cppca_include_catch, cppca_context);
#endif
}

/*****************************************************************************/
/* Program Entry Point                                                       */
/*****************************************************************************/
int main() {
	OPEN_LOG("test_MCN1_acmotor.ctr", false, 100);
	START_SCRIPT("MCN1_acmotor", true);

	run_tests();

	return !END_SCRIPT(true);
}

/*****************************************************************************/
/* Test Control                                                              */
/*****************************************************************************/
/* run_tests() contains calls to the individual test cases, you can turn test*/
/* cases off by adding comments*/
void run_tests() {
	test_ACM_Init(0);
	test_ACM_InitOnFly(0);
	test_ACM_ManageParking(0);
	test_ACM_InitControlStruct(0);
	test_ACM_UpdateControlStruct(0);
	test_ACM_DoMotorControl(0);
	test_ACM_DoFrequencyRampPU(0);
	test_ACM_DoFrequencyRampPU_001(1);
	test_ACM_DoFrequencyRampPU_002(1);
	test_ACM_DoFrequencyRampPU_003(1);
	test_ACM_DoFrequencyRampPU_004(1);
	test_ACM_DoFrequencyRampPU_005(1);
	test_ACM_GetMaximumVoltageApp(0);
	test_ACM_SetVoltageVoverF(0);
	test_ACM_RsTempCorr(0);
	test_ACM_ComputeDeltaMax(0);
	test_ACM_ComputeDeltaAngle(0);
	test_ACM_WindMillLoop(0);
	test_ACM_DeltaLimiterLoop(0);
	test_ACM_MaxIpkLoop(0);
	test_ACM_MaxPbattLoop(0);
	test_ACM_TODLoop(0);
	test_ACM_PhaseVoltageLoop(0);
	test_ACM_CheckWindmillRecovery(0);
	test_ACM_CheckWindmillRecovery_001(1);
	test_ACM_CheckWindmillRecovery_002(1);
	test_SetFrequencySetPoint(0);
	test_ACM_SetFrequencySetPointFixed(0);
	test_ACM_Set_u16FreqRampPU(0);
	test_ACM_Get_u16FreqRampPU(0);
	test_ACM_Set_u16FreqRampSmoothPU(0);
	test_ACM_Get_u16FreqRampSmoothPU(0);
	test_ACM_ManagePulseCanc(0);

	rule_set("*", "*");
	EXPORT_COVERAGE("test_MCN1_acmotor.cov", cppca_export_replace);
}

/*****************************************************************************/
/* Test Cases                                                                */
/*****************************************************************************/

void test_ACM_CheckWindmillRecovery_002(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		BOOL returnValue;
		BOOL expected_returnValue;

		/* Set global data */
		initialise_global_data();
		VectVarFilt[3].s16NewOutput = 999;
		u8WMerrCounts = 1431655765;
		returnValue = 1431655765;

		/* Set expected values for global data checks */
		initialise_expected_global_data();
		expected_VectVarFilt[3].s16NewOutput = 999;
		expected_u8WMerrCounts = 0;
		expected_returnValue = 0;

		START_TEST("test_ACM_CheckWindmillRecovery_002",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS("MTC_GetScaledVoltage" "#s_r_700;"

		);

		/* Call SUT */
		returnValue = ACM_CheckWindmillRecovery();

		/* Test case checks */
		CHECK_S_INT(returnValue, expected_returnValue);
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_CheckWindmillRecovery_001(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		BOOL returnValue;
		BOOL expected_returnValue;

		/* Set global data */
		initialise_global_data();
		VectVarFilt[3].s16NewOutput = 7000;
		u8WMerrCounts = 14;
		returnValue = 1431655765;

		/* Set expected values for global data checks */
		initialise_expected_global_data();
		expected_VectVarFilt[3].s16NewOutput = 7000;
		expected_u8WMerrCounts = 15;
		expected_returnValue = 1;

		START_TEST("test_ACM_CheckWindmillRecovery_001",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS("MTC_GetScaledVoltage" "#s_r_1314;"
				"EMR_EmergencyDisablePowerStage" "#s_r_1;"

		);

		/* Call SUT */
		returnValue = ACM_CheckWindmillRecovery();

		/* Test case checks */
		CHECK_S_INT(returnValue, expected_returnValue);
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_DoFrequencyRampPU_005(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		u16 u16NewFreqPU;

		/* Set global data */
		initialise_global_data();
		VectVarFilt[0].s16NewOutput = 2;
		IbattStateMachine.BOOLForceGainFlag = 1431655765;
		BOOLSteadyStateFlag = 1431655765;
		u16NewFreqPU = 232;

		/* Set expected values for global data checks */
		initialise_expected_global_data();
		expected_VectVarFilt[0].s16NewOutput = 2;
		expected_IbattStateMachine.BOOLForceGainFlag = 0;
		expected_BOOLSteadyStateFlag = 1;

		START_TEST("test_ACM_DoFrequencyRampPU_005",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS("ACM_Get_u16FreqRampPU" "#s_r_232;"
				"ACM_Get_u16FreqRampPU" "#s_r_50;"
				"FIR_UpdateFilt" "#s_r_1_p_0_p_25;"
				"ACM_Set_u16FreqRampSmoothPU" "#s_r_0_p_4;"

		);

		/* Call SUT */
		ACM_DoFrequencyRampPU(u16NewFreqPU);

		/* Test case checks */

		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_DoFrequencyRampPU_004(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		u16 u16NewFreqPU;

		/* Set global data */
		initialise_global_data();
		VectVarFilt[0].s16NewOutput = 2;
		IbattStateMachine.BOOLForceGainFlag = 1431655765;
		BOOLSteadyStateFlag = 1431655765;
		u16NewFreqPU = 300;

		/* Set expected values for global data checks */
		initialise_expected_global_data();
		expected_VectVarFilt[0].s16NewOutput = 2;
		expected_IbattStateMachine.BOOLForceGainFlag = 1;
		expected_BOOLSteadyStateFlag = 0;

		START_TEST("test_ACM_DoFrequencyRampPU_004",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS("ACM_Get_u16FreqRampPU" "#s_r_333;"
				"ACM_Get_u16FreqRampPU" "#s_r_200;"
				"ACM_Set_u16FreqRampPU" "#s_r_1_p_188;"
				"ACM_Get_u16FreqRampPU" "#s_r_200;"
				"FIR_UpdateFilt" "#s_r_0_p_0_p_100;"
				"ACM_Set_u16FreqRampSmoothPU" "#s_r_0_p_4;"

		);

		/* Call SUT */
		ACM_DoFrequencyRampPU(u16NewFreqPU);

		/* Test case checks */

		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_DoFrequencyRampPU_003(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		u16 u16NewFreqPU;

		/* Set global data */
		initialise_global_data();
		VectVarFilt[0].s16NewOutput = 5;
		IbattReg.u16IbattRegOut = 0;
		IbattStateMachine.BOOLForceGainFlag = 1431655765;
		BOOLSteadyStateFlag = 1431655765;
		u16NewFreqPU = 234;

		/* Set expected values for global data checks */
		initialise_expected_global_data();
		expected_VectVarFilt[0].s16NewOutput = 5;
		expected_IbattReg.u16IbattRegOut = 0;
		expected_IbattStateMachine.BOOLForceGainFlag = 0;
		expected_BOOLSteadyStateFlag = 0;

		START_TEST("test_ACM_DoFrequencyRampPU_003",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS("ACM_Get_u16FreqRampPU" "#s_r_123;"
				"Temp_Get_BOOLSOATemperatureCorr" "#s_r_0;"
				"ACM_Get_u16FreqRampPU" "#s_r_50;"
				"ACM_Set_u16FreqRampPU" "#s_r_0_p_62;"
				"ACM_Get_u16FreqRampPU" "#s_r_50;"
				"FIR_UpdateFilt" "#s_r_50_p_0_p_25;"
				"ACM_Set_u16FreqRampSmoothPU" "#s_r_66_p_10;"

		);

		/* Call SUT */
		ACM_DoFrequencyRampPU(u16NewFreqPU);

		/* Test case checks */

		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_DoFrequencyRampPU_002(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		u16 u16NewFreqPU;

		/* Set global data */
		initialise_global_data();
		VectVarFilt[0].s16NewOutput = 2;
		IbattStateMachine.BOOLForceGainFlag = 1431655765;
		BOOLSteadyStateFlag = 1431655765;
		u16NewFreqPU = 110;

		/* Set expected values for global data checks */
		initialise_expected_global_data();
		expected_VectVarFilt[0].s16NewOutput = 2;
		expected_IbattStateMachine.BOOLForceGainFlag = 0;
		expected_BOOLSteadyStateFlag = 1;

		START_TEST("test_ACM_DoFrequencyRampPU_002",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS("ACM_Get_u16FreqRampPU" "#s_r_111;"
				"ACM_Get_u16FreqRampPU" "#s_r_99;"
				"FIR_UpdateFilt" "#s_r_1_p_0_p_49;"
				"ACM_Set_u16FreqRampSmoothPU" "#s_r_0_p_4;"

		);

		/* Call SUT */
		ACM_DoFrequencyRampPU(u16NewFreqPU);

		/* Test case checks */

		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_DoFrequencyRampPU_001(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		u16 u16NewFreqPU;

		/* Set global data */
		initialise_global_data();
		VectVarFilt[0].s16NewOutput = 15;
		IbattReg.u16IbattRegOut = 0;
		IbattStateMachine.BOOLForceGainFlag = 1431655765;
		BOOLSteadyStateFlag = 1431655765;
		u16NewFreqPU = 112;

		/* Set expected values for global data checks */
		initialise_expected_global_data();
		expected_VectVarFilt[0].s16NewOutput = 15;
		expected_IbattReg.u16IbattRegOut = 0;
		expected_IbattStateMachine.BOOLForceGainFlag = 0;
		expected_BOOLSteadyStateFlag = 1;

		START_TEST("test_ACM_DoFrequencyRampPU_001",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS("ACM_Get_u16FreqRampPU" "#s_r_111;"
				"Temp_Get_BOOLSOATemperatureCorr" "#s_r_0;"
				"ACM_Get_u16FreqRampPU" "#s_r_13;"
				"FIR_UpdateFilt" "#s_r_14_p_0_p_6;"
				"ACM_Set_u16FreqRampSmoothPU" "#s_r_16_p_30;"

		);

		/* Call SUT */
		ACM_DoFrequencyRampPU(u16NewFreqPU);

		/* Test case checks */

		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_Init(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_Init", "<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS(
				"{{MTC_ResetZCECntrl#default}{MTC_InitMTCVariables#default}{Tacho_InitTachoVariables#default}{MTC_InitBatteryDegragation#default}{ACM_InitControlStruct#default}{Power_InitPowerStruct#default}{REG_SetParPI#default}{REG_InitVarPI#default}{FIR_SetParFilt#default}{ACM_Get_u16FreqRampPU#default}{FIR_InitVarFilt#default}}");

		/* Call SUT */
		ACM_Init();

		/* Test case checks */
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_InitOnFly(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_InitOnFly", "<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS(
				"{{Power_InitPowerStruct#default}{RVP_SetRVPOn#default}{MTC_InitBatteryDegragation#default}{REG_SetParPI#default}{REG_InitVarPI#default}{FIR_SetParFilt#default}{ACM_Get_u16FreqRampPU#default}{FIR_InitVarFilt#default}}");

		/* Call SUT */
		ACM_InitOnFly();

		/* Test case checks */
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_ManageParking(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_ManageParking",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS(
				"{{TLE_EnableOutputs#default}{INT0_PWM_FailSafeMonitor#default}{WTD_ResetWatchdog#default}{INT0_PWM_FailSafeMonitor_rst#default}{RVP_SetRVPOn#default}{InputSP_GetOperatingMode#default}{RTI_SetMainTimeBase#default}{RTI_IsMainTimeElapsed#default}{INT0_FailSafeMonitor#default}{DIAG_ManageDiagnosticToECU#default}{INT0_FailSafeMonitor_rst#default}{MTC_SettingForParkingPhase#default}{Tacho_ResetTachoOverflowNumber#default}{MTC_ParkAngleSweep#default}}");

		/* Call SUT */
		ACM_ManageParking();

		/* Test case checks */
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_InitControlStruct(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_InitControlStruct",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS(
				"{{ACM_Set_u16FreqRampPU#default}{MTC_ComputeParkingTime#default}{ACM_Get_u16FreqRampPU#default}{ACM_Set_u16FreqRampSmoothPU#default}{ACM_Get_u16FreqRampSmoothPU#default}}");

		/* Call SUT */
		ACM_InitControlStruct();

		/* Test case checks */
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_UpdateControlStruct(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		u16 u16ActualBemf = CANTATA_DEFAULT_VALUE;
		u16 u16ActualFreq = CANTATA_DEFAULT_VALUE;
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_UpdateControlStruct",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS(
				"{{ACM_Set_u16FreqRampPU#default}{ACM_Get_u16FreqRampPU#default}{ACM_Set_u16FreqRampSmoothPU#default}{ACM_Get_u16FreqRampSmoothPU#default}}");

		/* Call SUT */
		ACM_UpdateControlStruct(u16ActualBemf, u16ActualFreq);

		/* Test case checks */
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_DoMotorControl(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_DoMotorControl",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS(
				"{{RTI_IsRegPeriodElapsed#default}{InputSP_GetInputSetPoint#default}{ACM_DoFrequencyRampPU#default}{MaxPower_PowerSetPoint#default}{ACM_ManagePulseCanc#default}{Tacho_ManageTachoCapture#default}{ACM_ComputeDeltaAngle#default}{Power_UpdatePowerStruct#default}{ACM_WindMillLoop#default}{ACM_DeltaLimiterLoop#default}{ACM_MaxPbattLoop#default}{ACM_SetVoltageVoverF#default}{ACM_MaxIpkLoop#default}{ACM_TODLoop#default}{ACM_PhaseVoltageLoop#default}{MTC_UpdateSine#default}}");

		/* Call SUT */
		ACM_DoMotorControl();

		/* Test case checks */
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_DoFrequencyRampPU(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		u16 u16NewFreqPU = CANTATA_DEFAULT_VALUE;
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_DoFrequencyRampPU",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS(
				"{{ACM_Get_u16FreqRampPU#default}{Temp_Get_BOOLSOATemperatureCorr#default}{ACM_Set_u16FreqRampPU#default}{FIR_UpdateFilt#default}{ACM_Set_u16FreqRampSmoothPU#default}}");

		/* Call SUT */
		ACM_DoFrequencyRampPU(u16NewFreqPU);

		/* Test case checks */
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_GetMaximumVoltageApp(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		u16 expected_returnValue = CANTATA_DEFAULT_VALUE;
		u16 returnValue;
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_GetMaximumVoltageApp",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS("");

		/* Call SUT */
		returnValue = ACM_GetMaximumVoltageApp();

		/* Test case checks */
		CHECK_U_INT(returnValue, expected_returnValue);
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_SetVoltageVoverF(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_SetVoltageVoverF",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS("{INT0_Get_strTempDrive_s16TempAmbReadPU#default}");

		/* Call SUT */
		ACM_SetVoltageVoverF();

		/* Test case checks */
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_RsTempCorr(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		u16 expected_returnValue = CANTATA_DEFAULT_VALUE;
		u16 returnValue;
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_RsTempCorr",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS("{INT0_Get_strTempDrive_s16TempAmbReadPU#default}");

		/* Call SUT */
		returnValue = ACM_RsTempCorr();

		/* Test case checks */
		CHECK_U_INT(returnValue, expected_returnValue);
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_ComputeDeltaMax(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		u16 expected_returnValue = CANTATA_DEFAULT_VALUE;
		u16 returnValue;
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_ComputeDeltaMax",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS("{ACM_RsTempCorr#default}");

		/* Call SUT */
		returnValue = ACM_ComputeDeltaMax();

		/* Test case checks */
		CHECK_U_INT(returnValue, expected_returnValue);
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_ComputeDeltaAngle(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_ComputeDeltaAngle",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS(
				"{{ACM_GetMaximumVoltageApp#default}{Curr_GetPeakCurrent#default}}");

		/* Call SUT */
		ACM_ComputeDeltaAngle();

		/* Test case checks */
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_WindMillLoop(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_WindMillLoop",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS(
				"{{FIR_UpdateFilt#default}{ACM_Get_u16FreqRampPU#default}{REG_UpdateInputPI#default}{REG_SetParPI#default}{REG_UpdateRegPI#default}{ACM_CheckWindmillRecovery#default}{Main_SetBit_u16ErrorType#default}{Main_ClrBit_u16ErrorType#default}{ACM_Get_u16FreqRampSmoothPU#default}}");

		/* Call SUT */
		ACM_WindMillLoop();

		/* Test case checks */
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_DeltaLimiterLoop(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_DeltaLimiterLoop",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS(
				"{{ACM_GetMaximumVoltageApp#default}{REG_SetParPI#default}{FIR_UpdateFilt#default}{ACM_Get_u16FreqRampPU#default}{REG_UpdateInputPI#default}{ACM_ComputeDeltaMax#default}{REG_UpdateRegPI#default}}");

		/* Call SUT */
		ACM_DeltaLimiterLoop();

		/* Test case checks */
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_MaxIpkLoop(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_MaxIpkLoop",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS(
				"{{REG_UpdateInputPI#default}{REG_UpdateRegPI#default}}");

		/* Call SUT */
		ACM_MaxIpkLoop();

		/* Test case checks */
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_MaxPbattLoop(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_MaxPbattLoop",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS(
				"{{REG_UpdateInputPI#default}{MaxPower_GetPowerSetPoint#default}{Power_Get_structPowerMeas_u32PbattPU#default}{REG_UpdateRegPI#default}{Temp_Get_BOOLSOATemperatureCorr#default}}");

		/* Call SUT */
		ACM_MaxPbattLoop();

		/* Test case checks */
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_TODLoop(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_TODLoop", "<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS(
				"{{FIR_UpdateFilt#default}{Power_Get_structPowerMeas_u32PbattPU#default}{REG_UpdateInputPI#default}{REG_SetParPI#default}{REG_UpdateRegPI#default}}");

		/* Call SUT */
		ACM_TODLoop();

		/* Test case checks */
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_PhaseVoltageLoop(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_PhaseVoltageLoop",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS(
				"{{ACM_Get_u16FreqRampPU#default}{REG_SetParPI#default}{REG_UpdateInputPI#default}{Curr_GetCurrent#default}{REG_UpdateRegPI#default}{MTC_GetScaledVoltage#default}{ACM_GetMaximumVoltageApp#default}}");

		/* Call SUT */
		ACM_PhaseVoltageLoop();

		/* Test case checks */
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_CheckWindmillRecovery(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		BOOL expected_returnValue = CANTATA_DEFAULT_VALUE;
		BOOL returnValue;
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_CheckWindmillRecovery",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS(
				"{{MTC_GetScaledVoltage#default}{EMR_EmergencyDisablePowerStage#default}}");

		/* Call SUT */
		returnValue = ACM_CheckWindmillRecovery();

		/* Test case checks */
		CHECK_U_CHAR(returnValue, expected_returnValue);
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_SetFrequencySetPoint(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		u8 u8InRef = CANTATA_DEFAULT_VALUE;
		u16 expected_returnValue = CANTATA_DEFAULT_VALUE;
		u16 returnValue;
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_SetFrequencySetPoint",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS("");

		/* Call SUT */
		returnValue = SetFrequencySetPoint(u8InRef);

		/* Test case checks */
		CHECK_U_INT(returnValue, expected_returnValue);
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_SetFrequencySetPointFixed(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		u8 u8InRef = CANTATA_DEFAULT_VALUE;
		u16 expected_returnValue = CANTATA_DEFAULT_VALUE;
		u16 returnValue;
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_SetFrequencySetPointFixed",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS("{SetFrequencySetPoint#default}");

		/* Call SUT */
		returnValue = ACM_SetFrequencySetPointFixed(u8InRef);

		/* Test case checks */
		CHECK_U_INT(returnValue, expected_returnValue);
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_Set_u16FreqRampPU(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		u16 value = CANTATA_DEFAULT_VALUE;
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_Set_u16FreqRampPU",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS("");

		/* Call SUT */
		ACM_Set_u16FreqRampPU(value);

		/* Test case checks */
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_Get_u16FreqRampPU(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		u16 expected_returnValue = CANTATA_DEFAULT_VALUE;
		u16 returnValue;
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_Get_u16FreqRampPU",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS("");

		/* Call SUT */
		returnValue = ACM_Get_u16FreqRampPU();

		/* Test case checks */
		CHECK_U_INT(returnValue, expected_returnValue);
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_Set_u16FreqRampSmoothPU(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		u16 value = CANTATA_DEFAULT_VALUE;
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_Set_u16FreqRampSmoothPU",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS("");

		/* Call SUT */
		ACM_Set_u16FreqRampSmoothPU(value);

		/* Test case checks */
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_Get_u16FreqRampSmoothPU(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		u16 expected_returnValue = CANTATA_DEFAULT_VALUE;
		u16 returnValue;
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_Get_u16FreqRampSmoothPU",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS("");

		/* Call SUT */
		returnValue = ACM_Get_u16FreqRampSmoothPU();

		/* Test case checks */
		CHECK_U_INT(returnValue, expected_returnValue);
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

void test_ACM_ManagePulseCanc(int doIt) {
	if (doIt) {
		/* Test case data declarations */
		/* Set global data */
		initialise_global_data();
		/* Set expected values for global data checks */
		initialise_expected_global_data();

		START_TEST("test_ACM_ManagePulseCanc",
				"<Insert test case description here>");

		/* Expected Call Sequence  */
		EXPECTED_CALLS("{INT0_Get_strTempDrive_s16TempAmbReadPU#default}");

		/* Call SUT */
		ACM_ManagePulseCanc();

		/* Test case checks */
		/* Checks on global data */
		check_global_data();
		END_CALLS();
		END_TEST();
	}
}

/*****************************************************************************/
/* Call Interface Control                                                    */
/*****************************************************************************/

/* Stub for function MTC_ResetZCECntrl */
void MTC_ResetZCECntrl() {
	REGISTER_CALL("MTC_ResetZCECntrl");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function MTC_InitMTCVariables */
void MTC_InitMTCVariables() {
	REGISTER_CALL("MTC_InitMTCVariables");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function Tacho_InitTachoVariables */
void Tacho_InitTachoVariables() {
	REGISTER_CALL("Tacho_InitTachoVariables");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function MTC_InitBatteryDegragation */
void MTC_InitBatteryDegragation() {
	REGISTER_CALL("MTC_InitBatteryDegragation");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function Power_InitPowerStruct */
void Power_InitPowerStruct() {
	REGISTER_CALL("Power_InitPowerStruct");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function REG_SetParPI */
void REG_SetParPI(PIenum enumReg, u16 u16IntGain, u16 u16PropGain,
		s32 s32RegLimLow, s32 s32RegLimHigh, BOOL BOOLFreeze, u16 u16IntStep) {
	REGISTER_CALL("REG_SetParPI");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function REG_InitVarPI */
void REG_InitVarPI(PIenum enumReg, s16 s16RegInput, s16 s16RegFeedback,
		s32 s32Integral, s32 s32Proportional, s32 s32RegOutput) {
	REGISTER_CALL("REG_InitVarPI");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function FIR_SetParFilt */
void FIR_SetParFilt(FiltEnum enumFilt, u16 u16SampleFreq, u16 u16PoleFreq,
		u16 u16ZeroFreq, s16 s16Hyst, u8 u8FiltType) {
	REGISTER_CALL("FIR_SetParFilt");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function FIR_InitVarFilt */
void FIR_InitVarFilt(FiltEnum enumFilt, s16 s16OldInput, s16 s16NewOutput) {
	REGISTER_CALL("FIR_InitVarFilt");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function RVP_SetRVPOn */
void RVP_SetRVPOn() {
	REGISTER_CALL("RVP_SetRVPOn");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function TLE_EnableOutputs */
void TLE_EnableOutputs() {
	REGISTER_CALL("TLE_EnableOutputs");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function INT0_PWM_FailSafeMonitor */
void INT0_PWM_FailSafeMonitor() {
	REGISTER_CALL("INT0_PWM_FailSafeMonitor");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function WTD_ResetWatchdog */
void WTD_ResetWatchdog() {
	REGISTER_CALL("WTD_ResetWatchdog");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function INT0_PWM_FailSafeMonitor_rst */
void INT0_PWM_FailSafeMonitor_rst() {
	REGISTER_CALL("INT0_PWM_FailSafeMonitor_rst");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function InputSP_GetOperatingMode */
OperatingMode_t InputSP_GetOperatingMode() {
	REGISTER_CALL("InputSP_GetOperatingMode");

	IF_INSTANCE("default") {
		return CANTATA_DEFAULT_VALUE;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return CANTATA_DEFAULT_VALUE;
}

/* Stub for function RTI_SetMainTimeBase */
void RTI_SetMainTimeBase(u8 u8Period) {
	REGISTER_CALL("RTI_SetMainTimeBase");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function RTI_IsMainTimeElapsed */
BOOL RTI_IsMainTimeElapsed() {
	REGISTER_CALL("RTI_IsMainTimeElapsed");

	IF_INSTANCE("default") {
		return CANTATA_DEFAULT_VALUE;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return CANTATA_DEFAULT_VALUE;
}

/* Stub for function INT0_FailSafeMonitor */
void INT0_FailSafeMonitor() {
	REGISTER_CALL("INT0_FailSafeMonitor");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function DIAG_ManageDiagnosticToECU */
void DIAG_ManageDiagnosticToECU(SystStatus_t enLocState, u16 u16DiagErrorType) {
	REGISTER_CALL("DIAG_ManageDiagnosticToECU");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function INT0_FailSafeMonitor_rst */
void INT0_FailSafeMonitor_rst() {
	REGISTER_CALL("INT0_FailSafeMonitor_rst");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function MTC_SettingForParkingPhase */
void MTC_SettingForParkingPhase() {
	REGISTER_CALL("MTC_SettingForParkingPhase");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function Tacho_ResetTachoOverflowNumber */
void Tacho_ResetTachoOverflowNumber() {
	REGISTER_CALL("Tacho_ResetTachoOverflowNumber");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function MTC_ParkAngleSweep */
BOOL MTC_ParkAngleSweep() {
	REGISTER_CALL("MTC_ParkAngleSweep");

	IF_INSTANCE("default") {
		return CANTATA_DEFAULT_VALUE;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return CANTATA_DEFAULT_VALUE;
}

/* Stub for function MTC_ComputeParkingTime */
u16 MTC_ComputeParkingTime() {
	REGISTER_CALL("MTC_ComputeParkingTime");

	IF_INSTANCE("default") {
		return CANTATA_DEFAULT_VALUE;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return CANTATA_DEFAULT_VALUE;
}

/* Stub for function RTI_IsRegPeriodElapsed */
BOOL RTI_IsRegPeriodElapsed() {
	REGISTER_CALL("RTI_IsRegPeriodElapsed");

	IF_INSTANCE("default") {
		return CANTATA_DEFAULT_VALUE;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return CANTATA_DEFAULT_VALUE;
}

/* Stub for function InputSP_GetInputSetPoint */
u8 InputSP_GetInputSetPoint() {
	REGISTER_CALL("InputSP_GetInputSetPoint");

	IF_INSTANCE("default") {
		return CANTATA_DEFAULT_VALUE;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return CANTATA_DEFAULT_VALUE;
}

/* Stub for function MaxPower_PowerSetPoint */
void MaxPower_PowerSetPoint() {
	REGISTER_CALL("MaxPower_PowerSetPoint");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function Tacho_ManageTachoCapture */
void Tacho_ManageTachoCapture() {
	REGISTER_CALL("Tacho_ManageTachoCapture");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function Power_UpdatePowerStruct */
void Power_UpdatePowerStruct() {
	REGISTER_CALL("Power_UpdatePowerStruct");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function MTC_UpdateSine */
BOOL MTC_UpdateSine(u16 u16NewVoltage, u16 u16NewFrequency) {
	REGISTER_CALL("MTC_UpdateSine");

	IF_INSTANCE("default") {
		return CANTATA_DEFAULT_VALUE;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return CANTATA_DEFAULT_VALUE;
}

/* Stub for function Temp_Get_BOOLSOATemperatureCorr */
BOOL Temp_Get_BOOLSOATemperatureCorr() {
	REGISTER_CALL("Temp_Get_BOOLSOATemperatureCorr");

	IF_INSTANCE("default") {
		return CANTATA_DEFAULT_VALUE;
	}

	IF_INSTANCE(
			"s_r_0"
	) {
		return 0;
	}

	IF_INSTANCE(
			"s_r_0"
	) {
		return 0;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return CANTATA_DEFAULT_VALUE;
}

/* Stub for function FIR_UpdateFilt */
void FIR_UpdateFilt(FiltEnum enumFilt, s16 s16NewInput) {
	REGISTER_CALL("FIR_UpdateFilt");

	IF_INSTANCE("default") {
		return;
	}

	IF_INSTANCE(
			"s_r_14_p_0_p_6"
	) {
		CHECK_S_INT(enumFilt, 0);
		CHECK_S_INT(s16NewInput, 6);
		return;
	}

	IF_INSTANCE(
			"s_r_1_p_0_p_49"
	) {
		CHECK_S_INT(enumFilt, 0);
		CHECK_S_INT(s16NewInput, 49);
		return;
	}

	IF_INSTANCE(
			"s_r_50_p_0_p_25"
	) {
		CHECK_S_INT(enumFilt, 0);
		CHECK_S_INT(s16NewInput, 25);
		return;
	}

	IF_INSTANCE(
			"s_r_0_p_0_p_100"
	) {
		CHECK_S_INT(enumFilt, 0);
		CHECK_S_INT(s16NewInput, 100);
		return;
	}

	IF_INSTANCE(
			"s_r_1_p_0_p_25"
	) {
		CHECK_S_INT(enumFilt, 0);
		CHECK_S_INT(s16NewInput, 25);
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function INT0_Get_strTempDrive_s16TempAmbReadPU */
s16 INT0_Get_strTempDrive_s16TempAmbReadPU() {
	REGISTER_CALL("INT0_Get_strTempDrive_s16TempAmbReadPU");

	IF_INSTANCE("default") {
		return CANTATA_DEFAULT_VALUE;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return CANTATA_DEFAULT_VALUE;
}

/* Stub for function Curr_GetPeakCurrent */
u16 Curr_GetPeakCurrent() {
	REGISTER_CALL("Curr_GetPeakCurrent");

	IF_INSTANCE("default") {
		return CANTATA_DEFAULT_VALUE;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return CANTATA_DEFAULT_VALUE;
}

/* Stub for function REG_UpdateInputPI */
void REG_UpdateInputPI(PIenum enumReg, s16 s16RegSp, s16 s16RegSample) {
	REGISTER_CALL("REG_UpdateInputPI");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function REG_UpdateRegPI */
void REG_UpdateRegPI(PIenum enumReg) {
	REGISTER_CALL("REG_UpdateRegPI");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function Main_SetBit_u16ErrorType */
void Main_SetBit_u16ErrorType(u16 errTyp) {
	REGISTER_CALL("Main_SetBit_u16ErrorType");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function Main_ClrBit_u16ErrorType */
void Main_ClrBit_u16ErrorType(u16 errTyp) {
	REGISTER_CALL("Main_ClrBit_u16ErrorType");

	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Stub for function MaxPower_GetPowerSetPoint */
u32 MaxPower_GetPowerSetPoint() {
	REGISTER_CALL("MaxPower_GetPowerSetPoint");

	IF_INSTANCE("default") {
		return CANTATA_DEFAULT_VALUE;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return CANTATA_DEFAULT_VALUE;
}

/* Stub for function Power_Get_structPowerMeas_u32PbattPU */
u32 Power_Get_structPowerMeas_u32PbattPU() {
	REGISTER_CALL("Power_Get_structPowerMeas_u32PbattPU");

	IF_INSTANCE("default") {
		return CANTATA_DEFAULT_VALUE;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return CANTATA_DEFAULT_VALUE;
}

/* Stub for function Curr_GetCurrent */
u16 Curr_GetCurrent() {
	REGISTER_CALL("Curr_GetCurrent");

	IF_INSTANCE("default") {
		return CANTATA_DEFAULT_VALUE;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return CANTATA_DEFAULT_VALUE;
}

/* Stub for function MTC_GetScaledVoltage */
u16 MTC_GetScaledVoltage() {
	REGISTER_CALL("MTC_GetScaledVoltage");

	IF_INSTANCE("default") {
		return CANTATA_DEFAULT_VALUE;
	}

	IF_INSTANCE(
			"s_r_1314"
	) {
		return 1314;
	}

	IF_INSTANCE(
			"s_r_700"
	) {
		return 700;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return CANTATA_DEFAULT_VALUE;
}

/* Stub for function EMR_EmergencyDisablePowerStage */
void EMR_EmergencyDisablePowerStage() {
	REGISTER_CALL("EMR_EmergencyDisablePowerStage");

	IF_INSTANCE("default") {
		return;
	}

	IF_INSTANCE(
			"s_r_1"
	) {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

#pragma qas cantata ignore on

/* Before-Wrapper for function ACM_InitControlStruct */
int BEFORE_ACM_InitControlStruct() {
	REGISTER_CALL("ACM_InitControlStruct");

	IF_INSTANCE("default") {
		return AFTER_WRAPPER;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return AFTER_WRAPPER;
}

/* After-Wrapper for function ACM_InitControlStruct */
void AFTER_ACM_InitControlStruct(struct cppsm_void_return cppsm_dummy) {
	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Replace-Wrapper for function ACM_InitControlStruct */
void REPLACE_ACM_InitControlStruct() {

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Before-Wrapper for function ACM_Get_u16FreqRampPU */
int BEFORE_ACM_Get_u16FreqRampPU() {
	REGISTER_CALL("ACM_Get_u16FreqRampPU");

	IF_INSTANCE("default") {
		return AFTER_WRAPPER;
	}

	IF_INSTANCE(
			"s_r_111"
	) {
		return REPLACE_WRAPPER;
	}

	IF_INSTANCE(
			"s_r_13"
	) {
		return REPLACE_WRAPPER;
	}

	IF_INSTANCE(
			"s_r_111"
	) {
		return REPLACE_WRAPPER;
	}

	IF_INSTANCE(
			"s_r_99"
	) {
		return REPLACE_WRAPPER;
	}

	IF_INSTANCE(
			"s_r_123"
	) {
		return REPLACE_WRAPPER;
	}

	IF_INSTANCE(
			"s_r_50"
	) {
		return REPLACE_WRAPPER;
	}

	IF_INSTANCE(
			"s_r_50"
	) {
		return REPLACE_WRAPPER;
	}

	IF_INSTANCE(
			"s_r_333"
	) {
		return REPLACE_WRAPPER;
	}

	IF_INSTANCE(
			"s_r_200"
	) {
		return REPLACE_WRAPPER;
	}

	IF_INSTANCE(
			"s_r_200"
	) {
		return REPLACE_WRAPPER;
	}

	IF_INSTANCE(
			"s_r_232"
	) {
		return REPLACE_WRAPPER;
	}

	IF_INSTANCE(
			"s_r_50"
	) {
		return REPLACE_WRAPPER;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return AFTER_WRAPPER;
}

/* After-Wrapper for function ACM_Get_u16FreqRampPU */
u16 AFTER_ACM_Get_u16FreqRampPU(u16 cppsm_return_value) {
	IF_INSTANCE("default") {
		return cppsm_return_value;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return cppsm_return_value;
}

/* Replace-Wrapper for function ACM_Get_u16FreqRampPU */
u16 REPLACE_ACM_Get_u16FreqRampPU() {

	IF_INSTANCE(
			"s_r_111"
	) {
		return 111;
	}

	IF_INSTANCE(
			"s_r_13"
	) {
		return 13;
	}

	IF_INSTANCE(
			"s_r_111"
	) {
		return 111;
	}

	IF_INSTANCE(
			"s_r_99"
	) {
		return 99;
	}

	IF_INSTANCE(
			"s_r_123"
	) {
		return 123;
	}

	IF_INSTANCE(
			"s_r_50"
	) {
		return 50;
	}

	IF_INSTANCE(
			"s_r_50"
	) {
		return 50;
	}

	IF_INSTANCE(
			"s_r_333"
	) {
		return 333;
	}

	IF_INSTANCE(
			"s_r_200"
	) {
		return 200;
	}

	IF_INSTANCE(
			"s_r_200"
	) {
		return 200;
	}

	IF_INSTANCE(
			"s_r_232"
	) {
		return 232;
	}

	IF_INSTANCE(
			"s_r_50"
	) {
		return 50;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return CANTATA_DEFAULT_VALUE;
}

/* Before-Wrapper for function ACM_Set_u16FreqRampPU */
int BEFORE_ACM_Set_u16FreqRampPU(u16 value) {
	REGISTER_CALL("ACM_Set_u16FreqRampPU");

	IF_INSTANCE("default") {
		return AFTER_WRAPPER;
	}

	IF_INSTANCE(
			"s_r_0_p_62"
	) {
		CHECK_S_INT(value, 62);
		return REPLACE_WRAPPER;
	}

	IF_INSTANCE(
			"s_r_1_p_188"
	) {
		CHECK_S_INT(value, 188);
		return REPLACE_WRAPPER;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return AFTER_WRAPPER;
}

/* After-Wrapper for function ACM_Set_u16FreqRampPU */
void AFTER_ACM_Set_u16FreqRampPU(struct cppsm_void_return cppsm_dummy,
		u16 value) {
	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Replace-Wrapper for function ACM_Set_u16FreqRampPU */
void REPLACE_ACM_Set_u16FreqRampPU(u16 value) {

	IF_INSTANCE(
			"s_r_0_p_62"
	) {
		CHECK_S_INT(value, 62);
		return 0;
	}

	IF_INSTANCE(
			"s_r_1_p_188"
	) {
		CHECK_S_INT(value, 188);
		return 1;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Before-Wrapper for function ACM_Set_u16FreqRampSmoothPU */
int BEFORE_ACM_Set_u16FreqRampSmoothPU(u16 value) {
	REGISTER_CALL("ACM_Set_u16FreqRampSmoothPU");

	IF_INSTANCE("default") {
		return AFTER_WRAPPER;
	}

	IF_INSTANCE(
			"s_r_16_p_30"
	) {
		CHECK_S_INT(value, 30);
		return REPLACE_WRAPPER;
	}

	IF_INSTANCE(
			"s_r_0_p_4"
	) {
		CHECK_S_INT(value, 4);
		return REPLACE_WRAPPER;
	}

	IF_INSTANCE(
			"s_r_66_p_10"
	) {
		CHECK_S_INT(value, 10);
		return REPLACE_WRAPPER;
	}

	IF_INSTANCE(
			"s_r_0_p_4"
	) {
		CHECK_S_INT(value, 4);
		return REPLACE_WRAPPER;
	}

	IF_INSTANCE(
			"s_r_0_p_4"
	) {
		CHECK_S_INT(value, 4);
		return REPLACE_WRAPPER;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return AFTER_WRAPPER;
}

/* After-Wrapper for function ACM_Set_u16FreqRampSmoothPU */
void AFTER_ACM_Set_u16FreqRampSmoothPU(struct cppsm_void_return cppsm_dummy,
		u16 value) {
	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Replace-Wrapper for function ACM_Set_u16FreqRampSmoothPU */
void REPLACE_ACM_Set_u16FreqRampSmoothPU(u16 value) {

	IF_INSTANCE(
			"s_r_16_p_30"
	) {
		CHECK_S_INT(value, 30);
		return 16;
	}

	IF_INSTANCE(
			"s_r_0_p_4"
	) {
		CHECK_S_INT(value, 4);
		return 0;
	}

	IF_INSTANCE(
			"s_r_66_p_10"
	) {
		CHECK_S_INT(value, 10);
		return 66;
	}

	IF_INSTANCE(
			"s_r_0_p_4"
	) {
		CHECK_S_INT(value, 4);
		return 0;
	}

	IF_INSTANCE(
			"s_r_0_p_4"
	) {
		CHECK_S_INT(value, 4);
		return 0;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Before-Wrapper for function ACM_Get_u16FreqRampSmoothPU */
int BEFORE_ACM_Get_u16FreqRampSmoothPU() {
	REGISTER_CALL("ACM_Get_u16FreqRampSmoothPU");

	IF_INSTANCE("default") {
		return AFTER_WRAPPER;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return AFTER_WRAPPER;
}

/* After-Wrapper for function ACM_Get_u16FreqRampSmoothPU */
u16 AFTER_ACM_Get_u16FreqRampSmoothPU(u16 cppsm_return_value) {
	IF_INSTANCE("default") {
		return cppsm_return_value;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return cppsm_return_value;
}

/* Replace-Wrapper for function ACM_Get_u16FreqRampSmoothPU */
u16 REPLACE_ACM_Get_u16FreqRampSmoothPU() {

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return CANTATA_DEFAULT_VALUE;
}

/* Before-Wrapper for function ACM_DoFrequencyRampPU */
int BEFORE_ACM_DoFrequencyRampPU(u16 u16NewFreqPU) {
	REGISTER_CALL("ACM_DoFrequencyRampPU");

	IF_INSTANCE("default") {
		return AFTER_WRAPPER;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return AFTER_WRAPPER;
}

/* After-Wrapper for function ACM_DoFrequencyRampPU */
void AFTER_ACM_DoFrequencyRampPU(struct cppsm_void_return cppsm_dummy,
		u16 u16NewFreqPU) {
	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Replace-Wrapper for function ACM_DoFrequencyRampPU */
void REPLACE_ACM_DoFrequencyRampPU(u16 u16NewFreqPU) {

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Before-Wrapper for function ACM_ManagePulseCanc */
int BEFORE_ACM_ManagePulseCanc() {
	REGISTER_CALL("ACM_ManagePulseCanc");

	IF_INSTANCE("default") {
		return AFTER_WRAPPER;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return AFTER_WRAPPER;
}

/* After-Wrapper for function ACM_ManagePulseCanc */
void AFTER_ACM_ManagePulseCanc(struct cppsm_void_return cppsm_dummy) {
	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Replace-Wrapper for function ACM_ManagePulseCanc */
void REPLACE_ACM_ManagePulseCanc() {

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Before-Wrapper for function ACM_ComputeDeltaAngle */
int BEFORE_ACM_ComputeDeltaAngle() {
	REGISTER_CALL("ACM_ComputeDeltaAngle");

	IF_INSTANCE("default") {
		return AFTER_WRAPPER;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return AFTER_WRAPPER;
}

/* After-Wrapper for function ACM_ComputeDeltaAngle */
void AFTER_ACM_ComputeDeltaAngle(struct cppsm_void_return cppsm_dummy) {
	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Replace-Wrapper for function ACM_ComputeDeltaAngle */
void REPLACE_ACM_ComputeDeltaAngle() {

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Before-Wrapper for function ACM_WindMillLoop */
int BEFORE_ACM_WindMillLoop() {
	REGISTER_CALL("ACM_WindMillLoop");

	IF_INSTANCE("default") {
		return AFTER_WRAPPER;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return AFTER_WRAPPER;
}

/* After-Wrapper for function ACM_WindMillLoop */
void AFTER_ACM_WindMillLoop(struct cppsm_void_return cppsm_dummy) {
	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Replace-Wrapper for function ACM_WindMillLoop */
void REPLACE_ACM_WindMillLoop() {

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Before-Wrapper for function ACM_DeltaLimiterLoop */
int BEFORE_ACM_DeltaLimiterLoop() {
	REGISTER_CALL("ACM_DeltaLimiterLoop");

	IF_INSTANCE("default") {
		return AFTER_WRAPPER;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return AFTER_WRAPPER;
}

/* After-Wrapper for function ACM_DeltaLimiterLoop */
void AFTER_ACM_DeltaLimiterLoop(struct cppsm_void_return cppsm_dummy) {
	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Replace-Wrapper for function ACM_DeltaLimiterLoop */
void REPLACE_ACM_DeltaLimiterLoop() {

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Before-Wrapper for function ACM_MaxPbattLoop */
int BEFORE_ACM_MaxPbattLoop() {
	REGISTER_CALL("ACM_MaxPbattLoop");

	IF_INSTANCE("default") {
		return AFTER_WRAPPER;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return AFTER_WRAPPER;
}

/* After-Wrapper for function ACM_MaxPbattLoop */
void AFTER_ACM_MaxPbattLoop(struct cppsm_void_return cppsm_dummy) {
	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Replace-Wrapper for function ACM_MaxPbattLoop */
void REPLACE_ACM_MaxPbattLoop() {

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Before-Wrapper for function ACM_SetVoltageVoverF */
int BEFORE_ACM_SetVoltageVoverF() {
	REGISTER_CALL("ACM_SetVoltageVoverF");

	IF_INSTANCE("default") {
		return AFTER_WRAPPER;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return AFTER_WRAPPER;
}

/* After-Wrapper for function ACM_SetVoltageVoverF */
void AFTER_ACM_SetVoltageVoverF(struct cppsm_void_return cppsm_dummy) {
	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Replace-Wrapper for function ACM_SetVoltageVoverF */
void REPLACE_ACM_SetVoltageVoverF() {

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Before-Wrapper for function ACM_MaxIpkLoop */
int BEFORE_ACM_MaxIpkLoop() {
	REGISTER_CALL("ACM_MaxIpkLoop");

	IF_INSTANCE("default") {
		return AFTER_WRAPPER;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return AFTER_WRAPPER;
}

/* After-Wrapper for function ACM_MaxIpkLoop */
void AFTER_ACM_MaxIpkLoop(struct cppsm_void_return cppsm_dummy) {
	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Replace-Wrapper for function ACM_MaxIpkLoop */
void REPLACE_ACM_MaxIpkLoop() {

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Before-Wrapper for function ACM_TODLoop */
int BEFORE_ACM_TODLoop() {
	REGISTER_CALL("ACM_TODLoop");

	IF_INSTANCE("default") {
		return AFTER_WRAPPER;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return AFTER_WRAPPER;
}

/* After-Wrapper for function ACM_TODLoop */
void AFTER_ACM_TODLoop(struct cppsm_void_return cppsm_dummy) {
	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Replace-Wrapper for function ACM_TODLoop */
void REPLACE_ACM_TODLoop() {

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Before-Wrapper for function ACM_PhaseVoltageLoop */
int BEFORE_ACM_PhaseVoltageLoop() {
	REGISTER_CALL("ACM_PhaseVoltageLoop");

	IF_INSTANCE("default") {
		return AFTER_WRAPPER;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return AFTER_WRAPPER;
}

/* After-Wrapper for function ACM_PhaseVoltageLoop */
void AFTER_ACM_PhaseVoltageLoop(struct cppsm_void_return cppsm_dummy) {
	IF_INSTANCE("default") {
		return;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Replace-Wrapper for function ACM_PhaseVoltageLoop */
void REPLACE_ACM_PhaseVoltageLoop() {

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return;
}

/* Before-Wrapper for function ACM_RsTempCorr */
int BEFORE_ACM_RsTempCorr() {
	REGISTER_CALL("ACM_RsTempCorr");

	IF_INSTANCE("default") {
		return AFTER_WRAPPER;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return AFTER_WRAPPER;
}

/* After-Wrapper for function ACM_RsTempCorr */
u16 AFTER_ACM_RsTempCorr(u16 cppsm_return_value) {
	IF_INSTANCE("default") {
		return cppsm_return_value;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return cppsm_return_value;
}

/* Replace-Wrapper for function ACM_RsTempCorr */
u16 REPLACE_ACM_RsTempCorr() {

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return CANTATA_DEFAULT_VALUE;
}

/* Before-Wrapper for function ACM_GetMaximumVoltageApp */
int BEFORE_ACM_GetMaximumVoltageApp() {
	REGISTER_CALL("ACM_GetMaximumVoltageApp");

	IF_INSTANCE("default") {
		return AFTER_WRAPPER;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return AFTER_WRAPPER;
}

/* After-Wrapper for function ACM_GetMaximumVoltageApp */
u16 AFTER_ACM_GetMaximumVoltageApp(u16 cppsm_return_value) {
	IF_INSTANCE("default") {
		return cppsm_return_value;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return cppsm_return_value;
}

/* Replace-Wrapper for function ACM_GetMaximumVoltageApp */
u16 REPLACE_ACM_GetMaximumVoltageApp() {

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return CANTATA_DEFAULT_VALUE;
}

/* Before-Wrapper for function ACM_CheckWindmillRecovery */
int BEFORE_ACM_CheckWindmillRecovery() {
	REGISTER_CALL("ACM_CheckWindmillRecovery");

	IF_INSTANCE("default") {
		return AFTER_WRAPPER;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return AFTER_WRAPPER;
}

/* After-Wrapper for function ACM_CheckWindmillRecovery */
BOOL AFTER_ACM_CheckWindmillRecovery(BOOL cppsm_return_value) {
	IF_INSTANCE("default") {
		return cppsm_return_value;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return cppsm_return_value;
}

/* Replace-Wrapper for function ACM_CheckWindmillRecovery */
BOOL REPLACE_ACM_CheckWindmillRecovery() {

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return CANTATA_DEFAULT_VALUE;
}

/* Before-Wrapper for function ACM_ComputeDeltaMax */
int BEFORE_ACM_ComputeDeltaMax() {
	REGISTER_CALL("ACM_ComputeDeltaMax");

	IF_INSTANCE("default") {
		return AFTER_WRAPPER;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return AFTER_WRAPPER;
}

/* After-Wrapper for function ACM_ComputeDeltaMax */
u16 AFTER_ACM_ComputeDeltaMax(u16 cppsm_return_value) {
	IF_INSTANCE("default") {
		return cppsm_return_value;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return cppsm_return_value;
}

/* Replace-Wrapper for function ACM_ComputeDeltaMax */
u16 REPLACE_ACM_ComputeDeltaMax() {

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return CANTATA_DEFAULT_VALUE;
}

/* Before-Wrapper for function SetFrequencySetPoint */
int BEFORE_SetFrequencySetPoint(u8 u8InRef) {
	REGISTER_CALL("SetFrequencySetPoint");

	IF_INSTANCE("default") {
		return AFTER_WRAPPER;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return AFTER_WRAPPER;
}

/* After-Wrapper for function SetFrequencySetPoint */
u16 AFTER_SetFrequencySetPoint(u16 cppsm_return_value, u8 u8InRef) {
	IF_INSTANCE("default") {
		return cppsm_return_value;
	}

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return cppsm_return_value;
}

/* Replace-Wrapper for function SetFrequencySetPoint */
u16 REPLACE_SetFrequencySetPoint(u8 u8InRef) {

	LOG_SCRIPT_ERROR("Call instance not defined.");
	return CANTATA_DEFAULT_VALUE;
}

#pragma qas cantata ignore off
/* pragma qas cantata testscript end */
/*****************************************************************************/
/* End of test script                                                        */
/*****************************************************************************/
