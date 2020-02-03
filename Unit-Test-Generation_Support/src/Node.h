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
};
