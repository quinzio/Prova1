#pragma once
#include <iostream>
#include <string>

#define BRACK_POS_LENGTH 6
struct coreType_str {
    int brackPos[BRACK_POS_LENGTH] = { -1, -1 ,-1 ,-1, -1, -1 };
    std::string coreType;
    std::string finalType;
    std::string core;
    bool fun;
};

std::vector<std::string> findCoreType(struct coreType_str& str, bool init = true);

