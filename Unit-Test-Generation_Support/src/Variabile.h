#pragma once
#include <string>
#include <vector>
#include <fstream>

#include "BbSize.h"

class Variable;

struct unionData {
    BbSize uSize;
    BbSize uOffset;
    Variable* myParent;
};

class Variable
{
public:
    [[maybe_unused]] std::string print(std::string prefix, std::string postfix);
    [[maybe_unused]] std::string print();
    static std::ofstream outputFile;

    std::string name;
    std::string hexID;
    unsigned long long value;
    int braceNested;
    int used = false;
    enum typeEnum_t {
        isArray,
        isRef,
        isValue,
        isStruct, 
        isUnion
    };
    enum typeEnum_t typeEnum;
    std::string type;
    Variable* pointsTo;
    std::vector<Variable> array;
    int arrayIx;
    std::vector<Variable> intStruct;
    std::vector<unsigned char> intUnion;
    struct unionData uData;
    Variable* myAddressDebug = NULL;


private:
};

class VariableShadow {
public:
    typedef std::vector<Variable> vVariable;
    std::vector<vVariable> shadows;
    VariableShadow() {
        vVariable v;
        shadows.push_back(v);
    }
};


