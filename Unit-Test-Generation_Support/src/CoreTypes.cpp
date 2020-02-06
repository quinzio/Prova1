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
    if (brackPosCnt > 0) {
        outCore = str.core.substr(0, str.brackPos[0]) + 
            str.core.substr((size_t)str.brackPos[1]+1, str.core.length() - str.brackPos[1]);
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
        str.core = str.core.substr(str.brackPos[0] + 1, str.brackPos[1] - str.brackPos[0] - 1);
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
