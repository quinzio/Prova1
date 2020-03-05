// testcompose.cpp : Questo file contiene la funzione 'main', in cui inizia e termina l'esecuzione del programma.
//

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <regex>
#include "json.hpp"

#include <stdio.h>
#include <stdlib.h>

std::string baseDir = "../../Unit-Test-Generation_Support/Unit-Test-Generation_Support/test/final/";

std::string testCase001 = R"(
void test_xxxx1xxxx(int doIt) {
    if (doIt) {
        /* Test case data declarations */
        xxxx2xxxx
        /* Set global data */
        initialise_global_data();
        xxxx7xxxx
        /* Set expected values for global data checks */
        initialise_expected_global_data();
        xxxx8xxxx

        START_TEST("test_xxxx3xxxx",
            "<Insert test case description here>");

        /* Expected Call Sequence  */
        EXPECTED_CALLS(
        xxxx4xxxx
        );

        /* Call SUT */
        xxxx5xxxx

        /* Test case checks */
        xxxx6xxxx 
        /* Checks on global data */
        check_global_data();
        END_CALLS();
        END_TEST();
    }
}
)";
/*
xxxx1xxxx
Function name

xxxx2xxxx
Locals and return value.
    int par1 = CANTATA_DEFAULT_VALUE;
    int expected_returnValue = CANTATA_DEFAULT_VALUE;
    int returnValue;

xxxx3xxxx
Function name same as 1

xxxx4xxxx
Expected calls

xxxx5xxxx
Function call, ay be or may not be with return value
returnValue = fun1(par1);

xxxx6xxxx
If return a type, that's the check
CHECK_S_INT(returnValue, expected_returnValue);


*/


typedef char gchar;
#define g_malloc (gchar *)malloc
#define g_warning std::cout << 
#define g_return_val_if_fail(cond, val) if (cond == false) return val;

gchar*
g_strcompress(const gchar* source)
{
    const gchar* p = source, * octal;
    gchar* dest;
    gchar* q;

    g_return_val_if_fail(source != NULL, NULL);

    dest = g_malloc(strlen(source) + 1);
    q = dest;

    while (*p)
    {
        if (*p == '\\')
        {
            p++;
            switch (*p)
            {
            case '\0':
                g_warning("g_strcompress: trailing \\");
                goto out;
            case '0':  case '1':  case '2':  case '3':  case '4':
            case '5':  case '6':  case '7':
                *q = 0;
                octal = p;
                while ((p < octal + 3) && (*p >= '0') && (*p <= '7'))
                {
                    *q = (*q * 8) + (*p - '0');
                    p++;
                }
                q++;
                p--;
                break;
            case 'b':
                *q++ = '\b';
                break;
            case 'f':
                *q++ = '\f';
                break;
            case 'n':
                *q++ = '\n';
                break;
            case 'r':
                *q++ = '\r';
                break;
            case 't':
                *q++ = '\t';
                break;
            case 'v':
                *q++ = '\v';
                break;
            default:            /* Also handles \" and \\ */
                *q++ = *p;
                break;
            }
        }
        else
            *q++ = *p;
        p++;
    }
out:
    *q = 0;

    return dest;
}

int main()
{
    std::vector<std::string> testFileVec;
    std::ifstream testFilesStr(baseDir + "test_temp.c");
    std::string str1;
    std::stringstream ss, ssLine;
    std::string function;
    std::smatch sm_testCaseDecl;
    std::regex e_testCaseDecl;
    std::smatch sm_stubwr;
    std::regex e_stubwr(R"(\s+(BEFORE_|AFTER_|REPLACE_|)(\w[\w\d]+)\s*\()");
    int lastTest = 0;
    int lastTestIndex = 0;
    int testCaseIndex = 0;
    nlohmann::json j, jTemp, jIfInstance, jInsert;
    std::ifstream inFile;
    std::ofstream outFile;
    std::map<int, std::string> mInsert;

    inFile.open(baseDir + "valuesCalls.txt");
    if (inFile.good() == false) {
        std::cout << "Can't open file\n";
    }
    inFile >> j;
    inFile.close();

    if (testFilesStr.good() == false) {
        std::cout << "Error opening file\n";
    }
    while (getline(testFilesStr, str1)) {
        testFileVec.push_back(str1);
    }
    testFilesStr.close();
    int ix = 0;
    auto str = testFileVec.begin();
    /* Search begin of test function declarations */
    for ( str = testFileVec.begin(); str != testFileVec.end(); str++, ix++) {
        if (str->find("/* Prototypes for test functions */") != std::string::npos) {
            std::cout << "Found prototypes at " << ix << "\n";
            break;
        }
    }
    /* Find highest function occourence or find end of test functions declarations */
    function = j["targetName"].get<std::string>();
    e_testCaseDecl = std::regex(R"(void\s*test_)" + function + R"((?:_(\d+))?)");
    for (; str != testFileVec.end(); str++, ix++) {
        if (std::regex_search(*str, sm_testCaseDecl, e_testCaseDecl)) {
            if (sm_testCaseDecl[1].length() > 0) {
                if (lastTest < std::stoul(sm_testCaseDecl[1])) {
                    lastTest = std::stoul(sm_testCaseDecl[1]);
                    lastTestIndex = ix;
                    std::cout << "Found function at " << ix << "\n";
                }
            }
            else {
                lastTest = 0;
                lastTestIndex = ix;
                std::cout << "Found function at " << ix << "\n";
            }
        }
        if (str->find("/****") != std::string::npos) {
            std::cout << "Found end at " << ix << "\n";
            break;
        }
    }
    std::cout << lastTest;
    ss.str("");
    ss << 
        "void " << 
        "test_" << 
        j["targetName"].get<std::string>() << 
        "_" << std::setw(3) << 
        std::setfill('0') << 
        lastTest + 1 << 
        "(int);";
    mInsert[lastTestIndex+1] = ss.str();



    /* Find start of calls to test cases */
    for (; str != testFileVec.end(); str++, ix++) {
        if (str->find("/* Test Control") != std::string::npos) {
            std::cout << "Found start of test functions " << ix << "\n";
            break;
        }
    }
    /* Find call to test case with the last index*/
    ss.str(std::string());
    if (lastTest == 0) {
        ss << j["targetName"].get<std::string>();
    }
    else {
        ss << j["targetName"].get<std::string>() << "_" << std::setw(3) << std::setfill('0') << lastTest;
    }
    for (; str != testFileVec.end(); str++, ix++) {
        if (str->find(ss.str()) != std::string::npos) {
            std::cout << "test case with the last index " << ix << "\n";
            ss.str("");
            ss << "\ttest_" << 
                j["targetName"].get<std::string>() << 
                "_" <<
                std::setw(3) << 
                std::setfill('0') << lastTest + 1 << 
                "(1);";
            mInsert[ix+1] = ss.str();
            break;
        }
    }


    /* Place the new test case at the beginning */
    /* Build the replacement strings */
    std::string x1x;
    std::string x2x;
    std::string x3x;
    std::string x4x;
    std::string x5x;
    std::string x6x;
    std::string x7x;
    std::string x8x;

    ss.str(std::string());
    ss << std::setw(3) << std::setfill('0') << lastTest + 1;
    x1x = j["targetName"].get<std::string>() + "_" + ss.str();
    for (auto it = j["parameters"].begin(); it != j["parameters"].end(); it++) {
        x2x += (*it)["type"].get<std::string>() + " " + (*it)["name"].get<std::string>() + ";\n";
    }
    /* if == npos means not found */
    if (j["returnType"].get<std::string>().find("void") == std::string::npos) {
        x2x += "\t" + j["returnType"].get<std::string>() + " returnValue;\n";
        x2x += "\t" + j["returnType"].get<std::string>() + " expected_returnValue;\n";
    }
    x3x = x1x;
    jTemp = j["expectedCalls"]["list"];
    for (auto it = jTemp.begin(); it != jTemp.end(); it++) {
        std::string t = g_strcompress((*it).get<std::string>().c_str());
        x4x += t;
    }
    /* if == npos means not found */
    if (j["returnType"].get<std::string>().find("void") == std::string::npos) {
        x5x = "returnValue = ";
    }
    x5x += j["targetName"].get<std::string>() + "(";
    bool erasecomma = false;
    for (auto it = j["parameters"].begin(); it != j["parameters"].end(); it++) {
        x5x += " " + (*it)["name"].get<std::string>() + ",";
        erasecomma = true;
    }
    x5x.erase(x5x.end() - 1);
    x5x += ");";
    /* if == npos means not found */
    if (j["returnType"].get<std::string>().find("void") == std::string::npos) {
        x6x = "CHECK_S_INT(returnValue, expected_returnValue);";
    }

    std::ifstream inputValues(baseDir + "valuesInput.txt");
    std::string line;
    while (std::getline(inputValues, line)) {
        x7x += "\t " + line + "\n";
    }
    inputValues.close();

    std::ifstream expectedValues(baseDir + "valuesExpected.txt");
    while (std::getline(expectedValues, line)) {
        x8x += "\t " + line + "\n";
    }
    expectedValues.close();


    int pos = testCase001.find("xxxx1xxxx");
    testCase001.replace(pos, 9, x1x);
    pos = testCase001.find("xxxx2xxxx");
    testCase001.replace(pos, 9, x2x);
    pos = testCase001.find("xxxx3xxxx");
    testCase001.replace(pos, 9, x3x);
    pos = testCase001.find("xxxx4xxxx");
    testCase001.replace(pos, 9, x4x);
    pos = testCase001.find("xxxx5xxxx");
    testCase001.replace(pos, 9, x5x);
    pos = testCase001.find("xxxx6xxxx");
    testCase001.replace(pos, 9, x6x);
    pos = testCase001.find("xxxx7xxxx");
    testCase001.replace(pos, 9, x7x);
    pos = testCase001.find("xxxx8xxxx");
    testCase001.replace(pos, 9, x8x);

    std::cout << testCase001;

    /* Find start of test functions */
    for (; str != testFileVec.end(); str++) {
        if (str->find("/* Test Cases") != std::string::npos) {
            std::cout << "Found start of test functions " << ix << "\n";
            break;
        }
        ix++;
    }
    str++; ix++;
    str++; ix++;
    testCaseIndex = ix;
    mInsert[testCaseIndex] = testCase001;


    /* Now update the stub, and wrappers with the necessary IF_INSTANCES */
    /* Collect stubs wrappers, etc */
    for (auto it = j["expectedCalls"]["singleInstances"] .begin(); it != j["expectedCalls"]["singleInstances"].end(); it++) {
        std::string nameF = (*it)["name"].get<std::string>();
        if (jIfInstance[nameF].find("StubInstance") == jIfInstance[nameF].end()) {
            jIfInstance[nameF]["StubInstance"] = "";
        }
        if (jIfInstance[nameF].find("BeforeWrapInstance") == jIfInstance[nameF].end()) {
            jIfInstance[nameF]["BeforeWrapInstance"] = "";
        }
        if (jIfInstance[nameF].find("ReplaceWrapInstance") == jIfInstance[nameF].end()) {
            jIfInstance[nameF]["ReplaceWrapInstance"] = "";
        }

        if ((*it).find("StubInstance") != (*it).end()) {
            std::string stubs = jIfInstance[nameF]["StubInstance"].get<std::string>() + (*it)["StubInstance"].get<std::string>();
            jIfInstance[nameF]["StubInstance"] = stubs;
        }
        if ((*it).find("BeforeWrapInstance") != (*it).end()) {
            std::string befores = jIfInstance[nameF]["BeforeWrapInstance"].get<std::string>() + (*it)["BeforeWrapInstance"].get<std::string>();
            jIfInstance[nameF]["BeforeWrapInstance"] = befores;
        }
        if ((*it).find("ReplaceWrapInstance") != (*it).end()) {
            std::string replaces = jIfInstance[nameF]["ReplaceWrapInstance"].get<std::string>() + (*it)["ReplaceWrapInstance"].get<std::string>();
            jIfInstance[nameF]["ReplaceWrapInstance"] = replaces;
        }
    }

    /* Serach the beginning of the stub-wrappers sections */
    for (; str != testFileVec.end(); str++, ix++) {
        if (str->find("/* Call Interface Control") != std::string::npos) {
            std::cout << "Found start of stub-wrappers at " << ix << "\n";
            break;
        }
    }

    for (; str != testFileVec.end(); str++, ix++) {
        if (std::regex_search(*str, sm_stubwr, e_stubwr)) {
            /* mockType can be BEFORE_ AFTER_ REPLACE_ or nothing*/
            std::string mockType = sm_stubwr[1]; 
            std::string mockName = sm_stubwr[2];
            /* A mock is found, now search if we have instances to insert */
            if (jIfInstance.find(mockName) != jIfInstance.end()) {
                /* Find the insertion point line number */
                for (; str != testFileVec.end(); str++, ix++) {
                    if (str->find("LOG_SCRIPT_ERROR") != std::string::npos) {
                        std::cout << "Found LOG_SCRIPT_ERROR at " << ix << "\n";
                        break;
                    }
                }
                /* Copy the stub istances if it's a stub, ecc. */
                if (mockType.compare("") == 0 && 
                    jIfInstance[mockName].find("StubInstance") != jIfInstance[mockName].end()) 
                {
                    mInsert[ix] = jIfInstance[mockName]["StubInstance"].get<std::string>();
                }
                else if (mockType.compare("BEFORE_") == 0 && 
                    jIfInstance[mockName].find("BeforeWrapInstance") != jIfInstance[mockName].end()) 
                {
                    mInsert[ix] = jIfInstance[mockName]["BeforeWrapInstance"].get<std::string>();
                }
                else if (mockType.compare("AFTER_") == 0 && 
                    jIfInstance[mockName].find("AfterWrapInstance") != jIfInstance[mockName].end()) 
                {
                    mInsert[ix] = jIfInstance[mockName]["AfterWrapInstance"].get<std::string>();
                }
                else if (mockType.compare("REPLACE_") == 0 &&
                    jIfInstance[mockName].find("ReplaceWrapInstance") != jIfInstance[mockName].end())
                {
                    mInsert[ix] = jIfInstance[mockName]["ReplaceWrapInstance"].get<std::string>();
                }
            }
        }
    }

    std::ofstream debugFile1(baseDir + "debug1.txt");
    for (auto item : mInsert) {
        debugFile1 << std::setw(6) << item.first << ":\n " << item.second << "\n";
    }
    debugFile1.close();
    std::ofstream debugFile2(baseDir + "debug2.txt");
    debugFile2 << jIfInstance.dump(4);
    debugFile2.close();

    std::ofstream modifiedtestDrive(baseDir + "modifiedtestDrive.c", std::ofstream::trunc);
    if (modifiedtestDrive.good() == false) {
        std::cout << "Error opening modified test dive file.\n";
    }

    for (auto it = testFileVec.begin(); it != testFileVec.end(); it++) {
        if (mInsert.find(it - testFileVec.begin()) != mInsert.end()) {
                modifiedtestDrive << g_strcompress(mInsert[it - testFileVec.begin()].c_str()) << "\n";
        }
        modifiedtestDrive << *it << "\n";
    }
    modifiedtestDrive.close();

    return 0;

}

