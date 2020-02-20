#pragma once
#include <windows.h>
#include <string>

void testCompare(std::string testFolder);
extern HANDLE hEventReqGuiLine;
extern HANDLE hEventReqValue;

/*
Objects to be used by the GUI
*/
class forGui_c {
public:
    /* TestMachine --> GUI */
    std::string codeFileName;
    std::string astFileName;
    std::string valueFileName;
    int line;
    int col;
    int len;
    std::string varName;
    std::string strComm;

    /* GUI --> TestMachine */
    unsigned long long ValueFromGui;
};

extern forGui_c forGui;