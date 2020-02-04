#pragma once
#include <iostream>
#include <string>

struct coreType_str {
    int brackPos[6] = { -1, -1 ,-1 ,-1, -1, -1 };
    std::string coreType;
    std::string withoutFinalType;
    std::string finalType;
    std::string core;
    bool fun;
};

void findCoreType(struct coreType_str& str);

