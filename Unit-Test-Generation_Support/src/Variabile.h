#pragma once
#include <string>
#include <vector>
#include <fstream>

class Variable
{
public:
    [[maybe_unused]] std::string print(std::string prefix, std::string postfix);
    [[maybe_unused]] std::string print();
    static std::ofstream outputFile;

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
    std::vector<Variable> array;
    int arrayIx;
    std::vector<Variable> intStruct;


private:
};

