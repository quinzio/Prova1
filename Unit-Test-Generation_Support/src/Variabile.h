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
    static std::ofstream outputFileDir;
    /* The (partial) name of the variable*/
    std::string name;
    /* ID in hex base taken from ast */
    std::string hexID;
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
    /* The integer value */
    unsigned long long value = 0x55555555ULL;
    /* The double value */
    double valueDouble = 0.0;
    /* The integer valueset by user during a previous test */
    unsigned long long valueByUser = 0x55555555ULL;
    /* The double value set by user during a previous test */
    double valueDoubleByUser = 0.0;
    /* The integer default value */
    unsigned long long valueDefault = 0x55555555ULL;
    /* The double deafult value */
    double valueDoubleDefault = 0.0;
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
    /* If Variable is a function whis will tell if the function has a body 
    Otherwise, the meaning is not defined */
    bool functionHasBody = false;
    /* The vector of values that the function returns during the test. 
    A vector is necessary because the function may return different vaues durinf the test. 
    Each function has its own counter so that adding or removing a function to the test 
    doesn't affect the return list of the othervariables. */
    std::vector<Variable> functionReturns;
    /* During the test the function is accessed many times, so this keeps track of how 
    many times the function was accessed. */
    int funcRetIndex;
    /* Public methods */
    void updateCommonArea(void);
    void updateUnion();
    [[maybe_unused]] std::string print(std::string prefix, std::string postfix, bool replaceWSetByUser);
    [[maybe_unused]] std::string print2(std::string prefix, std::string nameComposite, std::string postfix, bool replaceWSetByUser);
    [[maybe_unused]] std::string print(bool replaceWSetByUser);

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


