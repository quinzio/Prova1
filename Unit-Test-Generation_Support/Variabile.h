#pragma once
#include <string>
#include <vector>

class Variable
{
public:
    Variable();
    ~Variable();
    [[maybe_unused]] std::string print(std::string prefix);
    [[maybe_unused]] std::string print();

    std::string name;
    unsigned long long value;
    int braceNested;
    int used = false;
    enum typeEnum_t {
        isArray,
        isRef,
        isValue,
        isStruct
    };
    enum typeEnum_t typeEnum;
    std::string type;
    Variable* pointsTo;
    std::vector<Variable>* array = new std::vector<Variable>;
    int arrayIx;
    std::vector<Variable> intStruct;


private:
};

