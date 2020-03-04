/*****************************************************************************/
/*                            Cantata Test Script                            */
/*****************************************************************************/
/*
 *    Filename: test_temp.c
 *    Author: MUNARID
 *    Generated on: 04-Mar-2020 17:09:42
 *    Generated from: temp.c
 */
/*****************************************************************************/
/* Environment Definition                                                    */
/*****************************************************************************/

#define TEST_SCRIPT_GENERATOR 2

/* Include files from software under test */

#define CANTATA_DEFAULT_VALUE 0 /* Default value of variables & stub returns */

#include <cantpp.h>  /* Cantata Directives */
/* pragma qas cantata testscript start */
/*****************************************************************************/
/* Global Data Definitions                                                   */
/*****************************************************************************/

/* Global Functions */
extern int f2(int f2a, int f2b, int f2c);
extern int fun1(int par1);
int f1(int f1a, int f1b, int f1c);
int f3(int f3a, int f3b, int f3c);

/* Global data */
extern int a;
extern int b;
extern int c;
extern int d[3];

/* Expected variables for global data */
int expected_a;
int expected_b;
int expected_c;
int expected_d[3];

/* This function initialises global data to default values. This function       */
/* is called by every test case so must not contain test case specific settings */
static void initialise_global_data(){
    TEST_SCRIPT_WARNING("Verify initialise_global_data()\n");
    INITIALISE(a);
    INITIALISE(b);
    INITIALISE(c);
    INITIALISE(d);
}

/* This function copies the global data settings into expected variables for */
/* use in check_global_data(). It is called by every test case so must not   */
/* contain test case specific settings.                                      */
static void initialise_expected_global_data(){
    TEST_SCRIPT_WARNING("Verify initialise_expected_global_data()\n");
    COPY_TO_EXPECTED(a, expected_a);
    COPY_TO_EXPECTED(b, expected_b);
    COPY_TO_EXPECTED(c, expected_c);
    COPY_TO_EXPECTED(d, expected_d);
}

/* This function checks global data against the expected values. */
static void check_global_data(){
    TEST_SCRIPT_WARNING("Verify check_global_data()\n");
    CHECK_S_INT(a, expected_a);
    CHECK_S_INT(b, expected_b);
    CHECK_S_INT(c, expected_c);
    CHECK_MEMORY("d", d, expected_d, sizeof(expected_d));
}

/* Prototypes for test functions */
void run_tests();
void test_f2(int);
void test_fun1(int);

/*****************************************************************************/
/* Coverage Analysis                                                         */
/*****************************************************************************/
/* Coverage Rule Set: 100% Entry Point + Statement + Call + Decision Coverage */
static void rule_set(char* cppca_sut,
                     char* cppca_context)
{
#ifdef CANTPP_SUBSET_DEFERRED_ANALYSIS
    TEST_SCRIPT_WARNING("Coverage Rule Set ignored in deferred analysis mode\n");
#elif CANTPP_INSTRUMENTATION_DISABLED
    TEST_SCRIPT_WARNING("Instrumentation has been disabled\n");
#else
    ANALYSIS_CHECK("100% Entry Point Coverage",
                   cppca_entrypoint_cov,
                   100.0);

    ANALYSIS_CHECK("100% Statement Coverage",
                   cppca_statement_cov,
                   100.0);

    ANALYSIS_CHECK("100% Call Return Coverage",
                   cppca_callreturn_cov,
                   100.0);

    ANALYSIS_CHECK("100% Decision Coverage",
                   cppca_decision_cov,
                   100.0);

    REPORT_COVERAGE(cppca_entrypoint_cov|
                    cppca_statement_cov|
                    cppca_callreturn_cov|
                    cppca_decision_cov,
                    cppca_sut,
                    cppca_all_details|cppca_include_catch,
                    cppca_context);
#endif
}

/*****************************************************************************/
/* Program Entry Point                                                       */
/*****************************************************************************/
int main()
{
    OPEN_LOG("test_temp.ctr", false, 100);
    START_SCRIPT("temp", true);

    run_tests();

    return !END_SCRIPT(true);
}

/*****************************************************************************/
/* Test Control                                                              */
/*****************************************************************************/
/* run_tests() contains calls to the individual test cases, you can turn test*/
/* cases off by adding comments*/
void run_tests()
{
    test_f2(1);
    test_fun1(1);

    rule_set("*", "*");
    EXPORT_COVERAGE("test_temp.cov", cppca_export_replace);
}

/*****************************************************************************/
/* Test Cases                                                                */
/*****************************************************************************/

void test_f2(int doIt){
if (doIt) {
    /* Test case data declarations */
    int f2a = CANTATA_DEFAULT_VALUE;
    int f2b = CANTATA_DEFAULT_VALUE;
    int f2c = CANTATA_DEFAULT_VALUE;
    int expected_returnValue = CANTATA_DEFAULT_VALUE;
    int returnValue;
    /* Set global data */
    initialise_global_data();
    /* Set expected values for global data checks */
    initialise_expected_global_data();

    START_TEST("test_f2",
               "<Insert test case description here>");

        /* Expected Call Sequence  */
        EXPECTED_CALLS("");

            /* Call SUT */
            returnValue = f2(f2a, f2b, f2c);

            /* Test case checks */
            CHECK_S_INT(returnValue, expected_returnValue);
            /* Checks on global data */
            check_global_data();
        END_CALLS();
    END_TEST();
}}

void test_fun1(int doIt){
if (doIt) {
    /* Test case data declarations */
    int par1 = CANTATA_DEFAULT_VALUE;
    int expected_returnValue = CANTATA_DEFAULT_VALUE;
    int returnValue;
    /* Set global data */
    initialise_global_data();
    /* Set expected values for global data checks */
    initialise_expected_global_data();

    START_TEST("test_fun1",
               "<Insert test case description here>");

        /* Expected Call Sequence  */
        EXPECTED_CALLS("{{f1#default}{f2#default}{f3#default}}");

            /* Call SUT */
            returnValue = fun1(par1);

            /* Test case checks */
            CHECK_S_INT(returnValue, expected_returnValue);
            /* Checks on global data */
            check_global_data();
        END_CALLS();
    END_TEST();
}}

/*****************************************************************************/
/* Call Interface Control                                                    */
/*****************************************************************************/

/* Stub for function f1 */
int f1(int f1a,
       int f1b,
       int f1c){
    REGISTER_CALL("f1");

    IF_INSTANCE("default") {
        return CANTATA_DEFAULT_VALUE;
    }

    LOG_SCRIPT_ERROR("Call instance not defined.");
    return CANTATA_DEFAULT_VALUE;
}

/* Stub for function f3 */
int f3(int f3a,
       int f3b,
       int f3c){
    REGISTER_CALL("f3");

    IF_INSTANCE("default") {
        return CANTATA_DEFAULT_VALUE;
    }

    LOG_SCRIPT_ERROR("Call instance not defined.");
    return CANTATA_DEFAULT_VALUE;
}

#pragma qas cantata ignore on

/* Before-Wrapper for function f2 */
int BEFORE_f2(int f2a,
              int f2b,
              int f2c){
    REGISTER_CALL("f2");

    IF_INSTANCE("default") {
        return AFTER_WRAPPER;
    }

    LOG_SCRIPT_ERROR("Call instance not defined.");
    return AFTER_WRAPPER;
}

/* After-Wrapper for function f2 */
int AFTER_f2(int cppsm_return_value,
             int f2a,
             int f2b,
             int f2c){
    IF_INSTANCE("default") {
        return cppsm_return_value;
    }

    LOG_SCRIPT_ERROR("Call instance not defined.");
    return cppsm_return_value;
}

/* Replace-Wrapper for function f2 */
int REPLACE_f2(int f2a,
               int f2b,
               int f2c){

    LOG_SCRIPT_ERROR("Call instance not defined.");
    return CANTATA_DEFAULT_VALUE;
}

#pragma qas cantata ignore off
/* pragma qas cantata testscript end */
/*****************************************************************************/
/* End of test script                                                        */
/*****************************************************************************/
