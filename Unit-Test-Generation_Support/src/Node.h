#pragma once
#include <string>

struct SourceRef_s {
    struct SourcePoint_s {
        std::string fileName;
        int line;
        int col;
    }ExtBegin, ExtEnd, Name;
};



class Node
{
public:
    enum ValueEnum_t {
        isInteger,
        isFloat, 
        isNothing
    };
    Node();
    ~Node();
    Node* prevSib = NULL;
    Node* nextSib = NULL;
    Node* child = NULL;
    Node* parent = NULL;
    int depth;
    std::string text;
    int astFileRow;
    std::string astType;
    struct SourceRef_s sourceRef;
    void setValue(unsigned long long value);
    unsigned long long getValue();
    void setValueDouble(float value);
    float getValueDouble();
    ValueEnum_t get_type();
private:
    unsigned long long value;
    float valueDouble;
    ValueEnum_t type;
};


