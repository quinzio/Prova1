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
    static std::ofstream outputFile;
    /* The (partial) name of the variable*/
    std::string name;
    /* ID in hex base taken from ast */
    std::string hexID;
    /* The integer value */
    unsigned long long value;
    /* The double value */
    double valueDouble;
    /* Which of the two values shoud I use ? Integer or float ? */
    enum ValueEnum_t {
        isInteger,
        isFloat
    };
    enum ValueEnum_t ValueEnum = Variable::ValueEnum_t::isInteger;
    /* The braces nesting level 0: global 1+: locals */
    int braceNested;
    /* Taken from ths ast: the variable is references (used) or not */
    bool used = false;
    /* Used in test. The user or the test itself wrote something in this variable */
    bool usedInTest = false;
    /* Variable was set by user during a previous test */
    bool setByUser = false;
    /* The integer valueset by user during a previous test */
    unsigned long long valueByUser;
    /* The double value set by user during a previous test */
    double valueDoubleByUser;
    /* The test has hit return, it's time to quit */
    int returnSignalled = false;
    /* Type of generic variable */
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
    /* The type as seen in the ast, in string form  */
    std::string type;
    /* If this is a pointer, it points to... */
    Variable* pointsTo;
    /* If it's a vector, elements are here */
    std::vector<Variable> array;
    /* Temporary store of the array index */
    int arrayIx;
    /* If it's a struct(union), here are memebers */
    std::vector<Variable> intStruct;
    /* If it's a union, this is the common area 
    that all memebers share */
    std::vector<unsigned char> intUnion;
    /* This struct stores offset and size of union members 
    It-s not as easy as in structures. */
    struct unionData uData;
    /* The address, in test machine memory of this very object */
    Variable* myAddressDebug = NULL;

    /* Public methods */
    void updateCommonArea(void);
    void updateUnion();
    [[maybe_unused]] std::string print(std::string prefix, std::string postfix);
    [[maybe_unused]] std::string print();

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


