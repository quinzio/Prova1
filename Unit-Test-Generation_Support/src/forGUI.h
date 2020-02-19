#pragma once
#include <windows.h>
#include <string>

void testCompare(std::string testFolder);
extern HANDLE hEventReqGuiLine;

/*
Objects to be used by the GUI
*/
class forGui_c {
public:
    int line;
    int col;
    int len;
    std::string varName;
    std::string filename;
};

extern forGui_c forGui;