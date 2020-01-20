// Prova1.cpp : Questo file contiene la funzione 'main', in cui inizia e termina l'esecuzione del programma.
//

#include <iostream>
#include <fstream>
#include <regex>
#include <iomanip>
#include <vector>
#include <string>
#include <set>
#include <utility>

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
    std::string astType;
};

Node::Node()
{
    depth = 0;
}

Node::~Node()
{
}

class Variable
{
public:
    Variable();
    ~Variable();
    std::string name;
    unsigned long long value;
    std::string type;
    Variable* pointsTo;
    int pointDepth;

private:
};

Variable::Variable()
{
}

Variable::~Variable()
{
}

std::vector<Variable> vVariable;

unsigned long long visit(Node* node);
unsigned long long cast(std::string str, unsigned long long v);

int main()
{
    std::cout << "Hello World!\n";
    if (std::regex_match("subject", std::regex("(sub)(.*)")))
        std::cout << "string literal matched\n";
    //std::string s("|-TypedefDecl 0xd2eaa8 <<invalid sloc>> <invalid sloc> implicit __NSConstantString 'struct __NSConstantString_tag");
    std::string s("[[[[aaaaa");
    std::regex e("(\\W*)");
    std::smatch sm;
    std::regex_search(s, sm, e);
    std::cout << sm[0] << "\n";
    std::cout << sm[1] << "\n";
    std::cout << sm[2] << "\n";

    std::ifstream infile("Text.txt");
    std::string str;
    std::regex eCatchGlobals    (R"(([^\w<]*)([\w<]+).*)");
    std::smatch smCatchGlobals;
    std::regex eCatchGlobalName (R"###([^\w<]*[\w<]+\s0x[\da-f]{6,7}\s<[^>]*>\s(?:col:\d+|line:\d+:\d+)(?:\sused)?\s(\w+)\s'([^']+)')###");
    std::smatch smCatchGlobalName;
    int depth = -2;
    Node myRoot;
    Node* cursor;
    cursor = &myRoot;
    //
    std::set<std::string> astTypesSet;

    while (std::getline(infile, str)) {
        if (std::regex_search(str, smCatchGlobals, eCatchGlobals) && smCatchGlobals.size() >= 2) {
            if  (   (   smCatchGlobals[1].length() == 2  // Will found only functions and global variables and skip local ones
                        && 
                            (smCatchGlobals[2].compare("VarDecl") == 0 
                            || 
                            smCatchGlobals[2].compare("FunctionDecl") == 0))
                    ||
                    (   smCatchGlobals[1].length() == 8
                        &&  // These are local variables
                        smCatchGlobals[2].compare("VarDecl") == 0)
                )
            {
                if (std::regex_search(str, smCatchGlobalName, eCatchGlobalName)) {
                    if (smCatchGlobals[1].length() == 2)
                        std::cout << "Found global  ";
                    else 
                        std::cout << "  Found local ";
                    std::cout << "name:type " << std::setw(30) << smCatchGlobalName[1] << ":" << smCatchGlobalName[2] << "\n";
                }
                else
                {
                    std::cout << "Not found global.  ";
                    std::cout << str << "\n";
                }
            }
            astTypesSet.insert(smCatchGlobals[2]);
            Node* tempNode = new Node();
            tempNode->text = str;
            tempNode->astType = smCatchGlobals[2];
            // Did I find a sibling (on the same indentation level) ?
            if (smCatchGlobals[1].length() <= depth) {
                while (smCatchGlobals[1].length() < depth) {
                    cursor = cursor->parent;
                    depth = cursor->depth;
                }
                tempNode->parent = cursor->parent;
                tempNode->prevSib = cursor;
                tempNode->depth = depth;
                cursor->nextSib = tempNode;
                cursor = tempNode;
            }
            // Did I find a new child (on a deeper indentation level) ?
            if (smCatchGlobals[1].length() > depth) {
                depth = smCatchGlobals[1].length();
                tempNode->parent = cursor;
                tempNode->depth = depth;
                cursor->child = tempNode;
                cursor = tempNode;
            }
        }
    }
    /*
        Print AST types encountered
    */
    for (auto it = astTypesSet.begin(); it != astTypesSet.end(); it++) {
        std::cout << *it << "\n";
    }
    /*
        Visit the AST
    */
    visit(&myRoot);
}

unsigned long long visit(Node *node)
{
    std::regex eIntegralType(R"###([^\w<]*[\w<]+\s0x[\da-f]{6,7}\s<[^>]*>\s'([^']+)'\s(\d+))###");
    std::smatch smIntegralType;
    std::regex eVarDecl(R"###([^\w<]*[\w<]+\s0x[\da-f]{6,7}\s<[^>]*>\s(?:col:\d+|line:\d+:\d+)(?:\sused)?\s(\w+)\s'([^']+)')###");
    std::smatch smVarDecl;
    std::regex eDeclRefExpr(R"###([^\w<]*[\w<]+\s0x[\da-f]{6,7}\s<[^>]*>\s'([^']+)'\slvalue\sVar\s0x[\da-f]{6,7}\s'(\w+)')###");
    std::smatch smDeclRefExpr;

    /*
        <<<NULL  (>>>)
        BinaryOperator
        BuiltinType
        CompoundStmt
        DeclRefExpr
        DeclStmt
        FunctionDecl
        IfStmt
        ImplicitCastExpr
        IntegerLiteral
        PointerType
        Record
        RecordType
        ReturnStmt
        TranslationUnitDecl
        TypedefDecl
        VarDecl
    */
    if (node->astType.compare("IntegerLiteral") == 0) {
        unsigned long long integral;
        unsigned long long ret;
        std::regex_search(node->text, smIntegralType, eIntegralType);
        integral = std::stoi(smIntegralType[2]);
        ret = cast(smIntegralType[1], integral);
        return ret;
    }
    if (node->astType.compare("VarDecl") == 0) {
        std::regex_search(node->text, smVarDecl, eVarDecl);
        Variable temp;
        temp.name = smVarDecl[1];
        temp.type = smVarDecl[2];
        temp.value = 0;
        vVariable.push_back(temp);
        return 0;
    }
    if (node->astType.compare("DeclRefExpr") == 0) {
        std::regex_search(node->text, smDeclRefExpr, eDeclRefExpr);
        std::string varName = smDeclRefExpr[2];
        for (auto it = vVariable.begin(); it != vVariable.end(); it++) {
            if (it->name.compare(varName) == 0) {
                return (unsigned long long)it._Ptr;
                
            }
        }
    }



    if (node->astType.compare("<<<NULL") == 0) {
        
        
    }

    if (node->child) {
        visit(node->child);
    }
    if (node->nextSib) {
        visit(node->nextSib);
    }
}

unsigned long long cast(std::string str, unsigned long long v) 
{
    if (str.compare("int") == 0) {
        return (int)v;
    }

    std::cout << "Exception encountered\n";
}
