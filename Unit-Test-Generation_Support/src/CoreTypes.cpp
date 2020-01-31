// types.cpp : Questo file contiene la funzione 'main', in cui inizia e termina l'esecuzione del programma.
//
#include <iostream>
#include <string>
#include "CoreTypes.h"

void findCoreType(struct coreType_str& str)
{
    int bracket = 0;
    int brackPosCnt = 0;
    if (str.brackPos[0] == -1)
    {    
        // Must be initialized
        str.brackPos[0] = 0;
        str.brackPos[1] = str.coreType.length();
        str.brackPos[2] = -1;
        str.brackPos[3] = -1;
        str.fun = false;
    }
    for (auto c = str.coreType.begin() + str.brackPos[0]; c < str.coreType.begin() + str.brackPos[1]; c++) {
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
    str.core = str.coreType.substr(str.brackPos[0], str.brackPos[1] - str.brackPos[0]);
    if (brackPosCnt == 2) {
        str.brackPos[0]++;
        str.brackPos[2] = 0;
        str.brackPos[3] = 0;
        findCoreType(str);
    }
    else if (brackPosCnt == 4) {
        str.fun = true;
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
