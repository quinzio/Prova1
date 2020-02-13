// types.cpp : Questo file contiene la funzione 'main', in cui inizia e termina l'esecuzione del programma.
//
#include <iostream>
#include <string>
#include <regex>
#include "CoreTypes.h"
#include "RegexCollCoreTypes.h"


void findCoreType(struct coreType_str& str, bool init)
{
    int bracket = 0;
    int brackPosCnt = 0;
    std::smatch smFinalType;
    std::smatch smRightBrack;
    std::smatch smRoundBrack;
    std::vector<std::string> typeChainB;
    std::string outCore;
    if (init)
    {   
        /* Try to remove unbalanced parenthesis 
        The unbalanced parenthesis should always have the form 
        ((([5][2][3][4]
        And only one of that for declaration
        */
        int unbalanced = 0;
        for (auto it = str.coreType.begin(); it != str.coreType.end(); it++) {
            if (*it == '(') unbalanced++;
            if (*it == ')') unbalanced--;
        }
        if (unbalanced > 0) {
            std::smatch smUnbalance;
            std::regex eUnbalance(
                "(.*)"
                "\\({" + std::to_string(unbalanced) + "}"
                "("
                "(?:\\[\\d+\\]){" + std::to_string(1) + "}"
                ")"
                "(.*)"
            );
            if (std::regex_search(str.coreType, smUnbalance, eUnbalance)) {
                str.coreType =
                    smUnbalance[1].str() +
                    smUnbalance[2].str() +
                    smUnbalance[3].str();
            }
            else throw;
        }
        /*
        Strip qualifiers as volatile, const, extern ?
        */
        std::regex_search(str.coreType, smFinalType, std::regex("(volatile|const)?(.*)"));
        str.coreType = smFinalType[2];
        /* 
        Must strip the final type which can be
        void (only with pointer ?)
        char
        unisgned char
        int
        unsigned int
        unsigned long long
        float
        struct str_name
        union str_name
        struct (anonymous struct at temp.c:11.12)
        typed_type (user defined with typedef)
        ecc
        */
        std::regex_search(str.coreType, smFinalType, eFinalType);
        str.finalType = smFinalType[1];
        // Initialize the core
        str.core = smFinalType[2];
        // Must be initialized
        str.brackPos[0] = -1;
        str.brackPos[1] = -1;
        str.brackPos[2] = -1;
        str.brackPos[3] = -1;
        str.fun = false;
    }
    if (str.core.length() > 0) {
        /* Due to a bug in clang ast output, you can see type declarations as this:
        'int ***(*(([4][3][2])[1][2][3]'
        If you watch carefully, you'll see that the parenthesis are not matched.
        To try to guess the balanced parenthesis, the following algorithm is implemented:
        1. Search for the first '(', this is the opening parenthesis
        2. Advance until the first ')', mark this as closing par.
        3. Continue scanning until another '(', mark every ')' as closing, 
            replacing the previous ')'.
        4. Repeat to find the matching '()' for the function calls parameters.
        */
        for (auto c = str.core.begin(); c < str.core.end(); c++) {
            if (*c == '(') {
                bracket++;
                if (bracket == 1) str.brackPos[brackPosCnt++] = c - str.core.begin();
            }
            else if (*c == ')') {
                bracket--;
                if (bracket == 0) str.brackPos[brackPosCnt++] = c - str.core.begin();
            }
            if (brackPosCnt == 5) {
                break;
            }
        }
    }
    // lastBrackPos[0] is the 1st "^", str.brackPos[0] is the 2nd "^"
    // lastBrackPos[1] is the 4th "^", str.brackPos[0] is the 3rd "^"
    // int *(**(*[6])[5][5])[4]
    //       ^ ^    ^      ^ 
    if (brackPosCnt > 0) {
        outCore = str.core.substr(0, str.brackPos[0])
            +
            str.core.substr(str.brackPos[1] + 1, str.core.length() - str.brackPos[1]);
        /* If in the outCore there are function parameters, we must strip them away*/
        std::regex eStripParams(R"(([^\(]*)\(.*\)(.*))");
        std::smatch smStripParams;
        if (std::regex_search(outCore, smStripParams, eStripParams)) {
            outCore = smStripParams[1].str() + smStripParams[2].str();
        }
    }
    else {
        outCore = str.core;
    }
    while (1) {
        std::regex_search(outCore, smRightBrack, eRightBrack);
        std::regex_search(outCore, smRoundBrack, eRoundBrack);
        if (smRightBrack.length(1) > 0) {
            typeChainB.push_back(smRightBrack[1]);
            outCore = smRightBrack.prefix().str() + smRightBrack.suffix().str();
        }
        else if (smRoundBrack.length(1) > 0) {
            typeChainB.push_back(smRoundBrack.str(1));
            outCore = smRoundBrack.prefix().str() + smRoundBrack.suffix().str();
        }
        else {
            break;
        }
    }
    if (brackPosCnt > 0) {
        str.core = str.core.substr(str.brackPos[0] + 1, str.brackPos[1] - (str.brackPos[0] + 1));
    }
    if (brackPosCnt == 2) {
        str.brackPos[0] = -1;
        str.brackPos[1] = -1;
        str.brackPos[2] = -1;
        str.brackPos[3] = -1;
        findCoreType(str, false);
    }
    else if (brackPosCnt == 4) {
        str.fun = true;
        str.brackPos[0] = -1;
        str.brackPos[1] = -1;
        str.brackPos[2] = -1;
        str.brackPos[3] = -1;
        findCoreType(str, false);
    }

    for (auto tC : typeChainB) {
        str.typeChainA.push_back(tC);
    }
    // If there was no parentheses in the type, we need to restore the final type
    // in front of the type, e.g. int, struct xxx, ecc..
    if (str.brackPos[0] == 0) {
        str.core = str.coreType;
    }
    return;
}


//void findCoreType(struct coreType_str& str, bool init)
//{
//    int bracket = 0;
//    int brackPosCnt = 0;
//    std::smatch smFinalType;
//    std::smatch smRightBrack;
//    std::smatch smRoundBrack;
//    std::vector<std::string> typeChainB;
//    std::string outCore;
//    if (init)
//    {    
//        /*
//        Strip qualifiers as volatile, const, extern ?
//        */
//        std::regex_search(str.coreType, smFinalType, std::regex("(volatile|const)?(.*)"));
//        str.coreType = smFinalType[2];
//        /* 
//        Must strip the final type which can be
//        void (only with pointer ?)
//        char
//        unisgned char
//        int
//        unsigned int
//        unsigned long long
//        float
//        struct str_name
//        union str_name
//        struct (anonymous struct at temp.c:11.12)
//        typed_type (user defined with typedef)
//        ecc
//        */
//        std::regex_search(str.coreType, smFinalType, eFinalType);
//        str.finalType = smFinalType[1];
//        // Initialize the core
//        str.core = smFinalType[2];
//        // Must be initialized
//        str.brackPos[0] = -1;
//        str.brackPos[1] = -1;
//        str.brackPos[2] = -1;
//        str.brackPos[3] = -1;
//        str.fun = false;
//    }
//    if (str.core.length() > 0) {
//        /* Due to a bug in clang ast output, you can see type declarations as this:
//        'int ***(*(([4][3][2])[1][2][3]'
//        If you watch carefully, you'll see that the parenthesis are not matched.
//        To try to guess the balanced parenthesis, the following algorithm is implemented:
//        1. Search for the first '(', this is the opening parenthesis
//        2. Advance until the first ')', mark this as closing par.
//        3. Continue scanning until another '(', mark every ')' as closing, 
//            replacing the previous ')'.
//        4. Repeat to find the matching '()' for the function calls parameters.
//        */
//        auto c = str.core.begin();
//        for (; c < str.core.end(); c++) {
//            if (*c == '(') {
//                brackPosCnt = 1;
//                if (bracket == 0) {
//                    str.brackPos[0] = c - str.core.begin();
//                }
//                bracket++;
//            }
//            else if (*c == ')') {
//                break;
//            }
//        }
//        for (; c < str.core.end(); c++) {
//            if (*c == ')') {
//                brackPosCnt = 2;
//                bracket--;
//                if (bracket >= 0) {
//                    str.brackPos[1] = c - str.core.begin();
//                }
//            }
//            else if (*c == '(') {
//                break;
//            }
//        }
//        bracket = 0;
//        for (; c < str.core.end(); c++) {
//            if (*c == '(') {
//                brackPosCnt = 3;
//                if (bracket == 0) {
//                    str.brackPos[2] = c - str.core.begin();
//                }
//                bracket++;
//            }
//            else if (*c == ')') {
//                break;
//            }
//        }
//        for (; c < str.core.end(); c++) {
//            if (*c == ')') {
//                brackPosCnt = 4;
//                bracket--;
//                if (bracket >= 0) {
//                    str.brackPos[3] = c - str.core.begin();
//                }
//            }
//            else if (*c == '(') {
//                break;
//            }
//        }
//    }
//    // adjust result in case the parenthesis are not balanced.
//    if (str.brackPos[0] > 0 && str.brackPos[1] == -1 && str.brackPos[2] == -1) {
//        str.brackPos[1] = str.core.length();
//        str.core += ")"; // Add the missing bracket
//        brackPosCnt = 2;
//    }
//    if (str.brackPos[0] > 0 && str.brackPos[1] == -1 && str.brackPos[2] > 0) {
//        str.brackPos[1] = str.brackPos[2] - 1;
//        brackPosCnt = 4;
//    }
//    if (brackPosCnt > 0) {
//        outCore = str.core.substr(0, str.brackPos[0]) + 
//            str.core.substr((size_t)str.brackPos[1]+1, str.core.length() - str.brackPos[1]);
//    }
//    else {
//        outCore = str.core;
//    }
//    while (1) {
//        std::regex_search(outCore, smRightBrack, eRightBrack);
//        std::regex_search(outCore, smRoundBrack, eRoundBrack);
//        if (smRightBrack.length(1) > 0) {
//            typeChainB.push_back(smRightBrack[1]);
//            outCore = smRightBrack.prefix().str() + smRightBrack.suffix().str();
//        }
//        else if (smRoundBrack.length(1) > 0) {
//            typeChainB.push_back(smRoundBrack.str(1));
//            outCore = smRoundBrack.prefix().str() + smRoundBrack.suffix().str();
//        }
//        else {
//            break;
//        }
//    }
//    if (brackPosCnt > 0) {
//        str.core = str.core.substr(str.brackPos[0] + 1, str.brackPos[1] - str.brackPos[0] - 1);
//    }
//    if (brackPosCnt == 2) {
//        str.brackPos[0] = -1;
//        str.brackPos[1] = -1;
//        str.brackPos[2] = -1;
//        str.brackPos[3] = -1;
//        findCoreType(str, false);
//    }
//    else if (brackPosCnt == 4) {
//        str.brackPos[0] = -1;
//        str.brackPos[1] = -1;
//        str.brackPos[2] = -1;
//        str.brackPos[3] = -1;
//        str.fun = true;
//        findCoreType(str, false);
//    }
//    for (auto tC : typeChainB) {
//        str.typeChainA.push_back(tC);
//    }
//    // If there was no parentheses in the type, we need to restore the final type
//    // in front of the type, e.g. int, struct xxx, ecc..
//    if (str.brackPos[0] == 0) {
//        str.core = str.coreType;
//    }
//    return;
//}
/*
int main() {
    //std::string str = R"###(int (*(*(*((*foo)(int, char (*)[2])))[4])[4])[4])###";
    std::string str = R"###(int *)###";
    struct coreType_str in;
    in.brackPos[0] = 0;
    in.brackPos[1] = str.length();
    in.brackPos[2] = -1;
    in.brackPos[3] = -1;
    in.coreType = str;
    in.fun = false;

    findCoreType(in);
}
*/
