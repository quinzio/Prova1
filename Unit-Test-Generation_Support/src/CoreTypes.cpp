// types.cpp : Questo file contiene la funzione 'main', in cui inizia e termina l'esecuzione del programma.
//
#include <iostream>
#include <string>
#include <regex>
#include "CoreTypes.h"
#include "RegexCollCoreTypes.h"

void findCoreType(struct coreType_str& str)
{
    int bracket = 0;
    int brackPosCnt = 0;
    std::smatch smFinalType;
    if (str.brackPos[0] == -1)
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
        str.withoutFinalType = smFinalType[2];
        // Initialize the core
        str.core = str.coreType;
        // Must be initialized
        str.brackPos[0] = 0;
        str.brackPos[1] = str.withoutFinalType.length();
        str.brackPos[2] = -1;
        str.brackPos[3] = -1;
        str.fun = false;
    }
    if (str.withoutFinalType.length() > 0) {
        for (auto c = str.withoutFinalType.begin() + str.brackPos[0]; c < str.withoutFinalType.begin() + str.brackPos[1]; c++) {
            if (*c == '(') {
                bracket++;
                if (bracket == 1) str.brackPos[brackPosCnt++] = c - str.coreType.begin();
            }
            else if (*c == ')') {
                bracket--;
                if (bracket == 0) str.brackPos[brackPosCnt++] = c - str.coreType.begin();
            }
            if (brackPosCnt == 5) {
                break;
            }
        }
        str.core = str.withoutFinalType.substr(str.brackPos[0], str.brackPos[1] - str.brackPos[0]);
    }
    if (brackPosCnt == 2) {
        str.brackPos[0]++;
        str.brackPos[2] = 0;
        str.brackPos[3] = 0;
        findCoreType(str);
    }
    else if (brackPosCnt == 4) {
        str.fun = true;
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
