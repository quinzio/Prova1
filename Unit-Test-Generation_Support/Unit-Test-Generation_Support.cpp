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
    int braceNested;
    int used = false;
    enum typeEnum_t{
        isArray,
        isRef,
        isValue
    };
    enum typeEnum_t typeEnum;
    std::string type;
    Variable* pointsTo;
    std::vector<Variable> *array = new std::vector<Variable>;
    int arrayIx;

private:
};

Variable::Variable()
{
}

Variable::~Variable()
{
}

std::vector<Variable> vVariable;

Variable visit(Node* node);
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

    std::ofstream pippo("pippo.txt");
    pippo << "pippo";
    pippo.close();

    std::ifstream infile("ex3/ast.txt");
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
    Variable t;
    t = visit(&myRoot);
}

Variable visit(Node *node)
{
    std::regex eIntegralType(R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s'([^']+)'\s(\d+))###");
    std::smatch smIntegralType;
    std::regex eVarDecl(R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s(?:col:\d+|line:\d+:\d+)(?:\sused)?\s(\w+)\s'([^']+)')###");
    std::smatch smVarDecl;
    std::regex eDeclRefExpr(R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s'([^']+)'\slvalue\sVar\s0x[\da-f]{6,11}\s'(\w+)')###");
    std::smatch smDeclRefExpr;
    std::regex eImplicitCastExpr(R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s'([^']+)'\s<([^>]*)>)###");
    std::smatch smImplicitCastExpr;
    std::regex eBinaryOperator(R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s'([^']+)'\s'([^']+)')###");
    std::smatch smBinaryOperator;
    std::regex eUnaryOperator(R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>\s'([^']+)'(?:\slvalue)?\s(postfix|prefix)\s'([^']+)')###");
    std::smatch smUnaryOperator;
    std::regex eDeclStmt(R"###([^\w<]*[\w<]+\s0x[\da-f]{6,11}\s<[^>]*>)###");
    std::smatch smDeclStmt;
    std::regex eArrayDecl(R"###(.*\[(\d+)\])###");
    std::smatch smArrayDecl;
    std::regex ePointerDecl(R"###(.*\s\*)###");
    std::smatch smPointerDecl;
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
        Variable ret;
        std::regex_search(node->text, smIntegralType, eIntegralType);
        integral = std::stoi(smIntegralType[2]);
        ret.value = cast(smIntegralType[1], integral);
        return ret;
    }

    if (node->astType.compare("VarDecl") == 0) {
        Variable temp;
        std::string rawType;
        std::regex_search(node->text, smVarDecl, eVarDecl);
        temp.name = smVarDecl[1];
        rawType = smVarDecl[2];
        temp.type = rawType;
        temp.used = true;
        std::regex_search(rawType, smArrayDecl, eArrayDecl);
        if (smArrayDecl.size() > 0) {
            int arrSize = std::stoi(smArrayDecl[1]);
            Variable a;
            a.typeEnum = Variable::typeEnum_t::isValue;
            temp.typeEnum = Variable::typeEnum_t::isArray;
            for (int i = 0; i < arrSize; i++) {
                a.value = i;
                temp.array->push_back(a);
            }
        }
        else if (std::regex_search(rawType, smPointerDecl, ePointerDecl)) {
            temp.typeEnum = Variable::typeEnum_t::isRef;
        }
        else {
            temp.typeEnum = Variable::typeEnum_t::isValue;
            temp.value = 0;
        }
        // Add Variable to store
        vVariable.push_back(temp);
        std::cout << "Value of variable " << temp.name << "\n";
        std::cin >> temp.value;
        return temp;
    }

    if (node->astType.compare("DeclRefExpr") == 0) {
        std::regex_search(node->text, smDeclRefExpr, eDeclRefExpr);
        std::string varName = smDeclRefExpr[2];
        Variable ret;
        for (auto it = vVariable.begin(); it != vVariable.end(); it++) {
            if (it->name.compare(varName) == 0) {
                ret.pointsTo = &*it;
                ret.typeEnum = Variable::typeEnum_t::isRef;
                return ret;
            }
        }
    }

    if (node->astType.compare("ImplicitCastExpr") == 0) {
        Variable ret;
        std::regex_search(node->text, smImplicitCastExpr, eImplicitCastExpr);
        std::string castTo = smImplicitCastExpr[1];
        /// Must always be a pointer to variable
        ret = visit(node->child);
        if (smImplicitCastExpr[2].compare("ArrayToPointerDecay") == 0) {
            // Leave the pointer as it is, it's already a pointer to array.
            return ret;
        }
        if (smImplicitCastExpr[2].compare("LValueToRValue") == 0) {
            if (ret.typeEnum == Variable::typeEnum_t::isArray) {
                return (*ret.pointsTo->array)[ret.pointsTo->arrayIx];
            }
            return *ret.pointsTo;
        }
        // No cast should be necessary as the type is not changed by 
        // ImplicitCastExpr
        return ret;
    }

    if (node->astType.compare("UnaryOperator") == 0) {
        Variable opa, ret;
        std::regex_search(node->text, smUnaryOperator, eUnaryOperator);
        std::string castTo = smUnaryOperator[1];
        std::string fix = smUnaryOperator[2];
        std::string uoperator = smUnaryOperator[3];
        opa = visit(node->child);
        if (uoperator.compare("++") == 0) {
            if (fix.compare("postfix") == 0) {
                ret = *opa.pointsTo;
            }
            opa.pointsTo->value++;
            if (fix.compare("prefix") == 0) {
                ret = *opa.pointsTo;
            }
        }
        if (uoperator.compare("--") == 0)  {
            if (fix.compare("postfix") == 0) {
                ret = *opa.pointsTo;
            }
            opa.pointsTo->value++;
            if (fix.compare("prefix") == 0) {
                ret = *opa.pointsTo;
            }
        }
        if (uoperator.compare("&") == 0) {
            ret = opa;
        }
        if (uoperator.compare("*") == 0) {
            ret = opa;
            ret.typeEnum = Variable::typeEnum_t::isRef;
        }
        return ret;
    }

    if (node->astType.compare("BinaryOperator") == 0) {
        std::regex_search(node->text, smBinaryOperator, eBinaryOperator);
        std::string castTo = smBinaryOperator[1];
        std::string boperator = smBinaryOperator[2];
        Variable opa, opb;
        Variable ret;
        opa = visit(node->child);
        opb = visit(node->child->nextSib);
        if (boperator.compare("<") == 0) {
            ret.value = opa.value < opb.value;
            return ret;
        }
        if (boperator.compare("=") == 0) {
            std::string saveName = opa.pointsTo->name;
            *opa.pointsTo = opb;
            opa.pointsTo->name = saveName;
            ret = *opa.pointsTo;
        }
        return ret;
    }
    if (node->astType.compare("IfStmt") == 0) {
        Variable cond, vtrue, vfalse;
        cond = visit(node->child->nextSib->nextSib);
        if (cond.value) {
            vtrue = visit(node->child->nextSib->nextSib->nextSib);
        }
        else {
            vfalse = visit(node->child->nextSib->nextSib->nextSib->nextSib);
        }
        return Variable();
    }

    if (node->astType.compare("ForStmt") == 0) {
        Node *init, *cond, *iter, *stmt;
        Variable vinit, vcond, viter, vstmt;
        init = node->child;
        cond = node->child->nextSib->nextSib;
        iter = node->child->nextSib->nextSib->nextSib;
        stmt = node->child->nextSib->nextSib->nextSib->nextSib;
        vinit = visit(init);
        while ((vcond = visit(cond)).value) {
            vstmt = visit(stmt);
            viter = visit(iter);
        }
        return Variable();
    }

    if (node->astType.compare("CompoundStmt") == 0) {
        Variable temp;
        Variable ret;
        for (auto next = node->child; next != NULL; next = next->nextSib) {
            temp = visit(next);
        }
        return ret;
    }

    if (node->astType.compare("TranslationUnitDecl") == 0) {
        Variable temp;
        for (auto next = node->child; next != NULL; next = next->nextSib) {
            temp = visit(next);
        }
        return Variable();
    }

    if (node->astType.compare("ArraySubscriptExpr") == 0) {
        Variable ret;
        Variable pArray = visit(node->child);
        int ix = visit(node->child->nextSib).value;
        ret.pointsTo = pArray.pointsTo->array->data() + ix;
        ret.typeEnum = Variable::typeEnum_t::isRef;
        return ret;
    }

    if (node->astType.compare("FunctionDecl") == 0) {
        Variable temp = visit(node->child);
        for (auto v : vVariable) {
            std::cout << "Value of variable " << v.name << ": " << v.value << "\n";
        }
        return temp;
    }

    if (node->astType.compare("DeclStmt") == 0) {
        Variable temp = visit(node->child);
        return temp;
    }

    if (node->astType.compare("<<<NULL") == 0) {
        return Variable();
    }

    if (node->astType.compare("TypedefDecl") == 0) {
        return Variable();
    }
    
    if (node->child) {
        Variable t;
        t = visit(node->child);
    }

    if (node->nextSib) {
        Variable t;
        t = visit(node->nextSib);
    }
    return Variable();
}

unsigned long long cast(std::string str, unsigned long long v) 
{
    if (str.compare("int") == 0) {
        return (int)v;
    }
    if (str.compare("unsigned int") == 0) {
        return (unsigned int)v;
    }

    std::cout << "Exception encountered\n";
}
