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
    double valueDouble;
    enum ValueEnum_t {
        isInteger,
        isFloat
    };
    enum ValueEnum_t ValueEnum = ValueEnum_t::isInteger;
    int braceNested;
    int used = false;
    int usedInTest = false;
    int returnSignalled = false;
    enum typeEnum_t {
        isArray,
        isRef,
        isValue,
        isStruct, 
        isUnion, 
        isEnum, 
        isFunction
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

    /* Public methods */
    void updateCommonArea(void);
    void updateUnion();

private:
};

class VariableShadow {
public:
    typedef std::vector<Variable*> vVariable;
    std::vector<vVariable> shadows;
    VariableShadow() {
        vVariable v;
        shadows.push_back(v);
    }
};


