#pragma once
#include <iostream>
#include <string>

#define BRACK_POS_LENGTH 6
struct coreType_str {
    std::vector<std::string> typeChainA;
    int brackPos[BRACK_POS_LENGTH] = { -1, -1 ,-1 ,-1, -1, -1 };
    std::string coreType;
    std::string finalType;
    bool declaredConst;
    std::string core;
    bool fun;
};

void findCoreType(struct coreType_str& str, bool init = true);
void stripParametersFunction(struct coreType_str& CoreTypes);
