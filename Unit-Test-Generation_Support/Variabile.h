#pragma once
#include <string>
#include <vector>

class Variable
{
public:
    Variable();
    ~Variable();
    std::string name;
    unsigned long long value;
    int braceNested;
    int used = false;
    enum typeEnum_t {
        isArray,
        isRef,
        isValue
    };
    enum typeEnum_t typeEnum;
    std::string type;
    Variable* pointsTo;
    std::vector<Variable>* array = new std::vector<Variable>;
    int arrayIx;

private:
};

Variable::Variable()
{
}

Variable::~Variable()
{
}