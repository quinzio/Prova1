/*
TODO: Take care of local variables shading global variables. 
You have already see a strange case, that made you lose a lot of time in 
debugging. 
*/

#include <iostream>
#include <fstream>
#include <regex>
#include <iomanip>
#include <vector>
#include <string>
#include <set>
#include <utility>
#include <algorithm> 
#include <cstdint>

#include "Unit-Test-Generation-Support.h"
#include "Variabile.h"
#include "Node.h"
#include "CoreTypes.h"
#include "RegexColl.h"
#include "SizeofTypes.h"

// vVariable contains all the global and local variables defined.
VariableShadow vShadowedVar;
// vStruct contains all the struct definition
std::vector<Variable> vStruct;
// vStruct contains all the union definition
std::vector<Variable> vUnion;
// vTypeDef contains all the user defined types
std::vector<Variable> vTypeDef;
std::ofstream Variable::outputFile;
int shadowLevel = 0;
struct SourceRef_s::SourcePoint_s globalSource;
extern std::string splash;
Variable* myP;

Variable visit(Node* node);
unsigned long long cast(std::string str, unsigned long long v);

int inner_main(int argc, std::string argv[]) throw (const std::exception&)
{
    if (argc != 3) {
        throw std::exception("\n\n*** There must be 2 arguments: ast file name and output file name. ***\n\n");
    }
    std::cout << "Unit Test Generation Support!\n";
    std::cout << splash;

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

    Variable::outputFile.open(argv[2], std::ios_base::trunc);
    if (!Variable::outputFile) throw std::exception("\n\n*** Cannot open output file. ***\n\n");

    std::ifstream infile(argv[1]);
    if (!infile) throw std::exception("\n\n*** Ast file not found. ***\n\n");
    std::string str;

    int depth = -2;
    int row = 1;
    Node myRoot;
    Node* cursor;
    cursor = &myRoot;
    //
    std::set<std::string> astTypesSet;
    std::smatch smCatchGlobals;
    std::smatch smCatchGlobalName;

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
                        std::cout << "Found global ";
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
            tempNode->astFileRow = row++;
            tempNode->astType = smCatchGlobals[2];
            // Get the source reference that is the first part of the string until 
            // the location
            setSourceLocations(tempNode, str);
            // Get the complete source location
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
    createBuiltInTypes();
    if (myRoot.child) 
        t = visit(myRoot.child);
     infile.close();
     Variable::outputFile.close();
    return 0;
}
void createBuiltInTypes(void)
{
    Variable temp;
    for (auto it = szTypes.begin(); it != szTypes.end(); it++) {
        temp.name = it->first;
        temp.type = it->first;
        temp.uData.uSize = BbSize(it->second, 0);
        temp.typeEnum = Variable::typeEnum_t::isValue;
        vTypeDef.push_back(temp);
    }
}
Variable visit(Node *node)
{
    if (node->astType.compare("IntegerLiteral") == 0) {
        return fIntegerLiteral(node);
    }
    if (node->astType.compare("VarDecl") == 0) {
        return fVarDecl(node);
    }
    if (node->astType.compare("TypedefDecl") == 0) {
        return fTypedefDecl(node);
    }
    if (node->astType.compare("DeclRefExpr") == 0) {
        return fDeclRefExpr(node);
    }
    if (node->astType.compare("ImplicitCastExpr") == 0) {
        return fImplicitCastExpr(node);
    }
    if (node->astType.compare("UnaryOperator") == 0) {
        return fUnaryOperator(node);
    }
    if (node->astType.compare("BinaryOperator") == 0) {
        return fBinaryOperator(node);
    }
    if (node->astType.compare("RecordDecl") == 0) {
        return fRecordDecl(node);
    }
    if (node->astType.compare("FieldDecl") == 0) {
        return fFieldDecl(node);
    }
    if (node->astType.compare("IndirectFieldDecl") == 0) {
        return fIndirectFieldDecl(node);
    }
    if (node->astType.compare("MemberExpr") == 0) {
        return fMemberExpr(node);
    }
    if (node->astType.compare("IfStmt") == 0) {
        return fIfStmt(node);
    }
    if (node->astType.compare("ForStmt") == 0) {
        return fForStmt(node);
    }
    if (node->astType.compare("CompoundStmt") == 0) {
        return fCompoundStmt(node);
    }
    if (node->astType.compare("TranslationUnitDecl") == 0) {
        return fTranslationUnitDecl(node);
    }
    if (node->astType.compare("ArraySubscriptExpr") == 0) {
        return fArraySubscriptExpr(node);
    }
    if (node->astType.compare("FunctionDecl") == 0) {
        return fFunctionDecl(node);
    }
    if (node->astType.compare("DeclStmt") == 0) {
        return fDeclStmt(node);
    }
    if (node->astType.compare("FullComment") == 0) {
        return fFullComment(node);
    }
    if (node->astType.compare("ConstantExpr") == 0) {
        return fConstantExpr(node);
    }
    if (node->astType.compare("<<<NULL") == 0) {
        return fNULL(node);
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
unsigned long long valueCast(std::string str, unsigned long long v) 
{
    if (str.compare("char") == 0) {
        return (int8_t)v;
    }
    else if (str.compare("signed char") == 0) {
        return (int8_t)v;
    }
    else if (str.compare("unsigned char") == 0) {
        return (uint8_t)v;
    }
    else if (str.compare("short") == 0) {
        return (int16_t)v;
    }
    else if (str.compare("short int") == 0) {
        return (int16_t)v;
    }
    else if (str.compare("signed short") == 0) {
        return (int16_t)v;
    }
    else if (str.compare("signed short int") == 0) {
        return (int16_t)v;
    }
    else if (str.compare("unsigned short") == 0) {
        return (uint16_t)v;
    }
    else if (str.compare("unsigned short int") == 0) {
        return (uint16_t)v;
    }
    else if (str.compare("int") == 0) {
        return (int16_t)v;
    }
    else if (str.compare("signed") == 0) {
        return (int16_t)v;
    }
    else if (str.compare("signed int") == 0) {
        return (int16_t)v;
    }
    else if (str.compare("unsigned") == 0) {
        return (uint16_t)v;
    }
    else if (str.compare("unsigned int") == 0) {
        return (uint16_t)v;
    }
    else if (str.compare("long") == 0) {
        return (int32_t)v;
    }
    else if (str.compare("signed long") == 0) {
        return (int32_t)v;
    }
    else if (str.compare("signed long int") == 0) {
        return (int32_t)v;
    }
    else if (str.compare("unsigned long") == 0) {
        return (uint32_t)v;
    }
    else if (str.compare("unsigned long int") == 0) {
        return (uint32_t)v;
    }
    else if (str.compare("long long") == 0) {
        return (int32_t)v;
    }
    else if (str.compare("long long int") == 0) {
        return (int32_t)v;
    }
    else if (str.compare("signed long long") == 0) {
        return (int32_t)v;
    }
    else if (str.compare("signed long long int") == 0) {
        return (int32_t)v;
    }
    else if (str.compare("unsigned long long") == 0) {
        return (uint32_t)v;
    }
    else if (str.compare("unsigned long long int") == 0) {
        return (uint32_t)v;
    }
    else if (str.compare("float") == 0) {
        return (float)v;
    }
    else if (str.compare("double") == 0) {
        return (double)v;
    }
    else if (str.compare("long double") == 0) {
        return (double)v;
    }
    /* At this point we have encountered a typedef cast 
    like a = 1; where a is a u8 a and u8 is a typedef. 
    So we search the typedef, and we get the type. 
    the "final" type should be a builtin or a pointer.
    Otherwise there shoukld be an explicit cast and that is handled otherwise.*/

    struct coreType_str coreTypes;
    coreTypes.coreType = str;
    findCoreType(coreTypes);
    /* Shouldn't be a pointer !!!
    So take only the final type */
    str = coreTypes.finalType;
    Variable temp;
    for (auto it = vTypeDef.begin(); it != vTypeDef.end(); it++) {
        if (it->name.compare(str) == 0) {
            temp = *it;
            break;
        }
    }
    // Try to cast again
    return valueCast(temp.type, v);



    throw std::exception("cast: Exception encountered\n");
}
void signExtend(Variable* v) {
    unsigned long long val;
    val = v->value;
    if (szTypes[v->type] == 1) {
        val & 0x80 ? 0xFFFFFFFF & val : val;
    }
    else if (szTypes[v->type] == 2) {
        val & 0x8000 ? 0xFFFFFFFF & val : val;
    }
}
Variable buildVariable(struct coreType_str& CoreTypes, Node* node) {
    bool found;
    std::smatch smTemp;
    std::smatch smStructTypeAnonymous;
    std::smatch smUnionTypeAnonymous;
    Variable temp;

    found = false;
    if (std::regex_search(CoreTypes.finalType, smTemp, std::regex("^struct\\s(.*)"))) {
        // Must take care of anonymous structs
        std::string typeName = smTemp[1];
        if (std::regex_search(typeName, smStructTypeAnonymous, eStructTypeAnonymous))
        {
            typeName = smStructTypeAnonymous[1];
        }
        for (auto t : vStruct) {
            if (t.name.compare(typeName) == 0) {
                temp = t;
                found = true;
            }
        }
    }
    else if (std::regex_search(CoreTypes.finalType, smTemp, std::regex("^union\\s(.*)"))) {
        // Must take care of anonymous unions ?
        std::string typeName = smTemp[1];
        if (std::regex_search(typeName, smUnionTypeAnonymous, eUnionTypeAnonymous))
        {
            typeName = smStructTypeAnonymous[1];
        }
        for (auto t : vUnion) {
            if (t.name.compare(typeName) == 0) {
                temp = t;
                found = true;
            }
        }
    }
    else if (std::regex_search(CoreTypes.finalType, smTemp, eBuiltinTypes)) {
        temp.type = CoreTypes.finalType;
        temp.typeEnum = Variable::typeEnum_t::isValue;
        temp.uData.uSize = szTypes[temp.type];
        found = true;
    }
    else {
        // Search into typedefs
        for (auto t : vTypeDef) {
            if (t.name.compare(CoreTypes.finalType) == 0) {
                temp = t;
                found = true;
            }
        }
    }
    if (found == false) {
        /* Try to find the typedef with the hexID in the (anon structs) */
        /* To understand the following just look at how a typedef
        ast node is made. It must refer to other typedefs.
        */
        Node* node2 = node->child;
        while (node2) {
            std::smatch smTemp;
            if (std::regex_search(node2->text, smTemp, std::regex(
                "[^\\w<]*"                       /*| |-              */
                "Record"                         /*Record            */
                "\\s"                            /*                  */
                "(0x[\\da-f]{6,11})"             /*0x247143d8308     */
            ))) {
                for (auto t : vStruct) {
                    if (t.hexID.compare(smTemp[1]) == 0) {
                        temp = t;
                        found = true;
                    }
                }
                if (found == false) {
                    for (auto t : vUnion) {
                        if (t.hexID.compare(smTemp[1]) == 0) {
                            temp = t;
                            found = true;
                        }
                    }
                }
            }
            while (node2->nextSib) {

                node2 = node2->nextSib;
            }
            node2 = node2->child;
        }
    }
    if (found == false) {
        throw std::exception("VarDecl: cannot find final type.");
    }


    int i;
    for (i = 0; i < CoreTypes.typeChainA.size(); i++) {
        if (CoreTypes.typeChainA.at(i).compare("*") == 0) {
            // It's omogeneous with the scan that end at i = CoreTypes.typeChainA.size()
            i++;
            break; // On the first pointer
        }
    }
    i--;
    for (; i >= 0; i--) {
        Variable vIt;
        if (std::regex_search(CoreTypes.typeChainA.at(i), smTemp, std::regex("\\[(\\d+)\\]"))) {
            /* An array */
            for (int ix = 0; ix < std::stoi(smTemp[1]); ix++) {
                vIt.typeEnum = Variable::typeEnum_t::isArray;
                vIt.array.push_back(temp);
            }
            vIt.uData.uSize = temp.uData.uSize * std::stoi(smTemp[1]);
            temp = vIt;
        }
        else if (std::regex_search(CoreTypes.typeChainA.at(i), smTemp, std::regex("\\*"))) {
            /* A pointer
               so the final type will be this pointer and not the previously found
               finalType. Reinitialize temp */
            temp = Variable();
            temp.typeEnum = Variable::typeEnum_t::isRef;
            temp.uData.uSize = { 4, 0 }; // !!!!! Is the size of pointer 4 ?
            temp.value = 0;
        }
    }
    return temp;
}

void recurseVariable(Variable* v, Variable* ref, void (*fp)(Variable*, Variable*)) {
    if (v->typeEnum == Variable::typeEnum_t::isRef) {
        fp(v, ref);
    }
    if (v->typeEnum == Variable::typeEnum_t::isArray) {
        for (auto it = v->array.begin(); it != v->array.end(); it++) {
            recurseVariable(&*it, ref, fp);
        }
    }
    if (v->typeEnum == Variable::typeEnum_t::isStruct) {
        for (auto it = v->intStruct.begin(); it != v->intStruct.end(); it++) {
            recurseVariable(&*it, ref, fp);
        }
    }
}
BbSize setVariableOffset(Variable* v, BbSize vOffset) {
    v->uData.myParent = myP;
    if (v->typeEnum == Variable::typeEnum_t::isArray) {
        for (auto it = v->array.begin(); it != v->array.end(); it++) {
            (&*it)->uData.uOffset = vOffset;
            vOffset = setVariableOffset(&*it, vOffset);
        }
    }
    else if (v->typeEnum == Variable::typeEnum_t::isStruct) {
        for (auto it = v->intStruct.begin(); it != v->intStruct.end(); it++) {
            (&*it)->uData.uOffset = vOffset;
            vOffset = setVariableOffset(&*it, vOffset);
        }
    }
    else if (v->typeEnum == Variable::typeEnum_t::isUnion) {
        for (auto it = v->intStruct.begin(); it != v->intStruct.end(); it++) {
            (&*it)->uData.uOffset = vOffset;
            setVariableOffset(&*it, vOffset);
        }
    }
    else {
        v->uData.uOffset = vOffset;
        vOffset += v->uData.uSize;
    }
    return vOffset;
}
void careUnions(Variable* v) {
    /* If v points to a union, update the parent address of all the members and 
    submembers.
    If v points to something which is part of a union, then updated the common area.
    */
    if (v->typeEnum == Variable::typeEnum_t::isUnion) {
        myP = v;
        setVariableOffset(v);
    }
    v->updateCommonArea();
}
void nullifyPointer(Variable* v, Variable* ref) {
    if (v->pointsTo == ref) {
        v->pointsTo = NULL;
    }
}
void cleanTestStorage() {
    VariableShadow::vVariable v;
    vShadowedVar.shadows.clear();
    vShadowedVar.shadows.push_back(v);
    vStruct.clear();
    vTypeDef.clear();
    shadowLevel = 0;
}
void setSourceLocations(Node* tempNode, std::string str) {
    std::smatch smSourceRef;
    std::smatch smSourcePoint;
    std::string temp;
    std::regex_search(str, smSourceRef, eSourceRef);
    if (smSourceRef.size() >= 3 && smSourceRef[2].matched) {
        temp = smSourceRef[2];
        std::regex_search(temp, smSourcePoint, eSourcePoint);
        setGlobalLocation(smSourcePoint);
    }
    tempNode->sourceRef.ExtBegin.fileName = globalSource.fileName;
    tempNode->sourceRef.ExtBegin.line = globalSource.line;
    tempNode->sourceRef.ExtBegin.col = globalSource.col;
    if (smSourceRef.size() >= 4 && smSourceRef[3].matched) {
        temp = smSourceRef[3];
        std::regex_search(temp, smSourcePoint, eSourcePoint);
        setGlobalLocation(smSourcePoint);
    }
    tempNode->sourceRef.ExtEnd.fileName = globalSource.fileName;
    tempNode->sourceRef.ExtEnd.line = globalSource.line;
    tempNode->sourceRef.ExtEnd.col = globalSource.col;
    if (smSourceRef.size() >= 5 && smSourceRef[4].matched) {
        temp = smSourceRef[4];
        std::regex_search(temp, smSourcePoint, eSourcePoint);
        setGlobalLocation(smSourcePoint);
    }
    tempNode->sourceRef.Name.fileName = globalSource.fileName;
    tempNode->sourceRef.Name.line = globalSource.line;
    tempNode->sourceRef.Name.col = globalSource.col;
}
void setGlobalLocation(std::smatch smSourcePoint) {
    if (smSourcePoint[6].matched) {
        globalSource.fileName = smSourcePoint[7];
        globalSource.line = std::stoi(smSourcePoint[8]);
        globalSource.col = std::stoi(smSourcePoint[9]);
    }
    else if (smSourcePoint[3].matched) {
        globalSource.line = std::stoi(smSourcePoint[4]);
        globalSource.col = std::stoi(smSourcePoint[5]);
    }
    else if (smSourcePoint[1].matched) {
        globalSource.col = std::stoi(smSourcePoint[2]);
    }
}
Variable getTypDef(std::string uType) {
    std::smatch smTypeDef;
    std::smatch smStructType;
    Variable ret;
    if (std::regex_search(uType, smTypeDef, eBuiltinTypes)) {
        // Built In type found
        ret.typeEnum = Variable::typeEnum_t::isValue;
        return ret;
    }
    for (auto t : vTypeDef) {
        if (t.name.compare(uType)) {
            return t;
        }
    }
    if (std::regex_search(uType, smStructType, eStructType)) {
        for (auto s : vStruct) {
            if (s.name.compare(smStructType[1])) {
                return s;
            }
        }
    }
    return ret;
}
Variable fVarDecl(Node* node) {
    Variable temp;
    std::string rawType;
    struct coreType_str CoreTypes;
    std::smatch smVarDecl;
    Variable* vback = NULL;
    std::regex_search(node->text, smVarDecl, eVarDecl);
    rawType = smVarDecl[2];
    /*
    Must extract the core type, example: struct s (*[3])[4]
    Core type is *[3]
    example: struct s (*(*[10])[3])[4]
    Core type is *[10]
    */
    CoreTypes.coreType = rawType; 
    findCoreType(CoreTypes);
    /* In CoreTypes.typeChainA there's now the vector of the types. 
       An array is made as of [11], a pointer is written as *, 
      (Function pointers are not implemented)
      In CoreTypes.finalType there is the final, the "real" type. 
      Can be an inbuilt type, as int or char, or void(must be preceded by a pointer), 
      struct, union, or a typedef.
      We allocate new Variables only to the first pointer. The correctness of the code
      has already been checked by the compiler. */

    temp = buildVariable(CoreTypes, node);
    temp.name = smVarDecl[1];
    temp.type = rawType;
    temp.used = true;

/*
//    if (smGenericType[5].length() > 0) {
//        int arrSize = std::stoi(smGenericType[5]);
//        Variable a;
//        a.typeEnum = Variable::typeEnum_t::isValue;
//        if (smGenericType[3].length() > 0) {
//            // example: int *[3], can be a struct or not
//            a.typeEnum = Variable::typeEnum_t::isRef;
//        }
//        else if (smGenericType[1].compare("struct") == 0) {
//            // example: struct s[3], without the pointer
//            // Must take care of anonymous structs
//            std::string typeName = smGenericType[2];
//            if (std::regex_search(typeName, smStructTypeAnonymous, eStructTypeAnonymous))
//            {
//                typeName = smStructTypeAnonymous[1];
//            }
//            for (auto t : vStruct) {
//                if (t.name.compare(typeName) == 0) {
//                    a = t;
//                }
//            }
//        }
//        temp.typeEnum = Variable::typeEnum_t::isArray;
//        for (int i = 0; i < arrSize; i++) {
//            a.value = i;
//            temp.array.push_back(a);
//        }
//    }
//    else if (smGenericType[3].length() > 0) {
//        // example: int *, it's not an array, but a pointer
//        temp.typeEnum = Variable::typeEnum_t::isRef;
//    }
//    else if (smGenericType[1].length() > 0) {
//        // example: struct s, it's neither an array not a pointer, but a struct
        // Must take care of anonymous structs
//        std::string typeName = smGenericType[2];
//        if (std::regex_search(typeName, smStructTypeAnonymous, eStructTypeAnonymous))
//        {
//            typeName = smStructTypeAnonymous[1];
//        }
//        for (Variable v : vStruct) {
//            if (v.name.compare(typeName) == 0) {
//                temp = v;
//                break;
//            }
//        }
//        temp.typeEnum = Variable::typeEnum_t::isStruct;
//        temp.name = smVarDecl[1];
//    }
//    else {
//        // example: int, it's neither an array nor a pointer nor a struct, but a basic type
//        temp.typeEnum = Variable::typeEnum_t::isValue;
//        temp.value = 0;
//    }
*/

    // Add Variable to store
    vShadowedVar.shadows[shadowLevel].push_back(temp);
    vback = &vShadowedVar.shadows[shadowLevel].back();
    vback->myAddressDebug = vback;
    myP = vback;
    setVariableOffset(vback);
    return *vback;
}
Variable fIntegerLiteral(Node* node) {
    unsigned long long integral;
    Variable ret;
    std::smatch smIntegralType;
    std::regex_search(node->text, smIntegralType, eIntegralType);
    integral = std::stoull(smIntegralType[2]);
    ret.value = valueCast(smIntegralType[1], integral);
    ret.type = smIntegralType[1];
    signExtend(&ret);
    ret.typeEnum = Variable::typeEnum_t::isValue;
    return ret;
}
Variable fTypedefDecl(Node* node) {
    /* This is virtually the same as fVarDecl. 
    I prefer to keep it separated. */
    Variable temp;
    std::string rawType;
    struct coreType_str CoreTypes;
    std::smatch smTypeDef;
    std::smatch smInvalidSloc;
    Variable* vback = NULL;
    /* First try to filter all those <<invalid sloc>>
    which are added by the compiler itself.
    */
    if (std::regex_search(node->text, smInvalidSloc, std::regex("<<invalid\\ssloc>>"))) {
        return temp;
    }
    std::regex_search(node->text, smTypeDef, eTypeDef);
    rawType = smTypeDef[3];
    /*
    Must extract the core type, example: struct s (*[3])[4]
    Core type is *[3]
    example: struct s (*(*[10])[3])[4]
    Core type is *[10]
    */
    CoreTypes.coreType = rawType;
    findCoreType(CoreTypes);
    /* In CoreTypes.typeChainA there's now the vector of the types.
       An array is made as of [11], a pointer is written as *,
      (Function pointers are not implemented)
      In CoreTypes.finalType there is the final, the "real" type.
      Can be an inbuilt type, as int or char, or void(must be preceded by a pointer),
      struct, union, or a typedef.
      We allocate new Variables only to the first pointer. The correctness of the code
      has already been checked by the compiler. */

    temp = buildVariable(CoreTypes, node);
    temp.name = smTypeDef[2];
    temp.type = rawType;
    temp.used = true;
    myP = &temp;
    setVariableOffset(&temp);

    /*
    //    if (smGenericType[5].length() > 0) {
    //        int arrSize = std::stoi(smGenericType[5]);
    //        Variable a;
    //        a.typeEnum = Variable::typeEnum_t::isValue;
    //        if (smGenericType[3].length() > 0) {
    //            // example: int *[3], can be a struct or not
    //            a.typeEnum = Variable::typeEnum_t::isRef;
    //        }
    //        else if (smGenericType[1].compare("struct") == 0) {
    //            // example: struct s[3], without the pointer
    //            // Must take care of anonymous structs
    //            std::string typeName = smGenericType[2];
    //            if (std::regex_search(typeName, smStructTypeAnonymous, eStructTypeAnonymous))
    //            {
    //                typeName = smStructTypeAnonymous[1];
    //            }
    //            for (auto t : vStruct) {
    //                if (t.name.compare(typeName) == 0) {
    //                    a = t;
    //                }
    //            }
    //        }
    //        temp.typeEnum = Variable::typeEnum_t::isArray;
    //        for (int i = 0; i < arrSize; i++) {
    //            a.value = i;
    //            temp.array.push_back(a);
    //        }
    //    }
    //    else if (smGenericType[3].length() > 0) {
    //        // example: int *, it's not an array, but a pointer
    //        temp.typeEnum = Variable::typeEnum_t::isRef;
    //    }
    //    else if (smGenericType[1].length() > 0) {
    //        // example: struct s, it's neither an array not a pointer, but a struct
            // Must take care of anonymous structs
    //        std::string typeName = smGenericType[2];
    //        if (std::regex_search(typeName, smStructTypeAnonymous, eStructTypeAnonymous))
    //        {
    //            typeName = smStructTypeAnonymous[1];
    //        }
    //        for (Variable v : vStruct) {
    //            if (v.name.compare(typeName) == 0) {
    //                temp = v;
    //                break;
    //            }
    //        }
    //        temp.typeEnum = Variable::typeEnum_t::isStruct;
    //        temp.name = smVarDecl[1];
    //    }
    //    else {
    //        // example: int, it's neither an array nor a pointer nor a struct, but a basic type
    //        temp.typeEnum = Variable::typeEnum_t::isValue;
    //        temp.value = 0;
    //    }
    */

    // Add TypeDef to store
    vTypeDef.push_back(temp);
    vback = &vTypeDef.back();
    vback->myAddressDebug = vback;
    return temp;
}
Variable fDeclRefExpr(Node* node) {
    std::smatch smDeclRefExpr;
    std::regex_search(node->text, smDeclRefExpr, eDeclRefExpr);
    std::string varName = smDeclRefExpr[2];
    Variable ret;
    for (auto it = vShadowedVar.shadows.rbegin(); it != vShadowedVar.shadows.rend(); it++) {
        for (auto itit = it->begin(); itit != it->end(); itit++) {
            if (itit->name.compare(varName) == 0) {
                ret.pointsTo = &*itit;
                ret.typeEnum = Variable::typeEnum_t::isRef;
                return ret;
            }
        }
    }
}
Variable fImplicitCastExpr(Node* node) {
    Variable ret;
    std::smatch smImplicitCastExpr;
    std::regex_search(node->text, smImplicitCastExpr, eImplicitCastExpr);
    std::string castTo = smImplicitCastExpr[1];
    /// Must always be a pointer to variable
    ret = visit(node->child);
    if (smImplicitCastExpr[2].compare("ArrayToPointerDecay") == 0) {
        // Leave the pointer as it is, it's already a pointer to array.
        return ret;
    }
    else if (smImplicitCastExpr[2].compare("LValueToRValue") == 0) {
        if (ret.typeEnum == Variable::typeEnum_t::isArray) {
            return ret.pointsTo->array[ret.pointsTo->arrayIx];
        }
        return *ret.pointsTo;
    }
    else if (smImplicitCastExpr[2].compare("IntegralCast") == 0) {
        ret.value = valueCast(smImplicitCastExpr[1].str(), ret.value);
        ret.type = smImplicitCastExpr[1];
        signExtend(&ret);
        return ret;
    }

    /* No cast should be necessary as the type is not changed by 
    ImplicitCastExpr */
    return ret;
}
Variable fUnaryOperator(Node* node) {
    Variable opa, ret;
    std::smatch smUnaryOperator;
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
    if (uoperator.compare("--") == 0) {
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
Variable fBinaryOperator(Node* node) {
    std::smatch smBinaryOperator;
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
        Variable * saveParent = opa.pointsTo->uData.myParent;
        struct unionData saveUData = opa.pointsTo->uData;
        *opa.pointsTo = opb;
        opa.pointsTo->uData.myParent = saveParent;
        opa.pointsTo->name = saveName;
        opa.pointsTo->uData = saveUData;
        careUnions(opa.pointsTo);
        ret = *opa.pointsTo;
    }
    return ret;
}
Variable fFieldDecl(Node* node) {
    Variable vField;
    std::smatch smFieldDeclaration;
    std::smatch smFieldDeclarationImplicit;
    std::smatch smStructTypeAnonymous;
    std::smatch smStructTypeAnonymous2;
    std::smatch smUnionTypeAnonymous;
    std::smatch smUnionTypeAnonymous2;
    std::smatch smStructType;
    std::string rawType;
    std::string name;
    std::string referenced;
    struct coreType_str coreTypes;
    bool found;
    if (std::regex_search(node->text, smFieldDeclarationImplicit, eFieldDeclarationImplicit)) {
        // Capture 1: "referenced" (or void)
        // Capture 2: type (will be anonymous)
        referenced = smFieldDeclarationImplicit[1];
        rawType = smFieldDeclarationImplicit[2];
    }
    else if (std::regex_search(node->text, smFieldDeclaration, eFieldDeclaration)) {
        // Capture 1: "referenced" (or void)
        // Capture 2: field name
        // Capture 3: type
        referenced = smFieldDeclaration[1];
        name = smFieldDeclaration[2];
        rawType = smFieldDeclaration[3];
    }
    if (std::regex_search(rawType, smStructTypeAnonymous, eStructTypeAnonymous)) {
        rawType = "struct " + smStructTypeAnonymous[1].str();
    }
    else if (std::regex_search(rawType, smStructTypeAnonymous2, eStructTypeAnonymous2)) {
        rawType = "struct " + smStructTypeAnonymous2[1].str();
    }
    else if (std::regex_search(rawType, smUnionTypeAnonymous, eUnionTypeAnonymous)) {
        rawType = "union " + smUnionTypeAnonymous[1].str();
    }
    else if (std::regex_search(rawType, smUnionTypeAnonymous2, eUnionTypeAnonymous2)) {
        rawType = "union " + smUnionTypeAnonymous2[1].str();
    }
    coreTypes.coreType = rawType;
    findCoreType(coreTypes);
    vField = buildVariable(coreTypes, node);
    vField.name = name;
    // Note the blank space in " referenced"
    if (smFieldDeclaration[1].compare(" referenced") == 0) {
        vField.used = true;
    }
    /* Search for bitfields. In case of bitfields the length will be that specified 
    by the bitfields.
    */
    if (node->child) {
        Variable bitField;
        bitField = visit(node->child);
        vField.uData.uSize = BbSize(0, bitField.value);
    }


    return vField;
}
Variable fRecordDecl(Node* node) {
    /*
    This function is for struct and for unions as well !!!
    */
    Variable tStruct, vField;
    std::smatch smStructDefinition;
    std::regex_search(node->text, smStructDefinition, eStructDefinition);
    /* Capture 1 is the hexadecimal ID of the node
       Capture 2 is struct or union 
       Capture 3 is the name of the struct */
    // Anonimous structures will take name as file.ext:line:col
    tStruct.name = smStructDefinition[3];
    /* The hexID will take a hexadecimal value
    It is necessary because when you make a typedef out of an 
    anonymous struct, the only way to find the struct is by its
    hexID */
    tStruct.hexID = smStructDefinition[1]; 
    if (tStruct.name.length() == 0) {
        tStruct.name =
            node->sourceRef.Name.fileName +
            ":" + std::to_string(node->sourceRef.Name.line) +
            ":" + std::to_string(node->sourceRef.Name.col);
    }
    if (smStructDefinition[2].compare("struct") == 0) {
        tStruct.typeEnum = Variable::typeEnum_t::isStruct;
    }
    else if (smStructDefinition[2].compare("union") == 0) {
        tStruct.typeEnum = Variable::typeEnum_t::isUnion;
        /* As we're allocating a union, we have to foresee the 
        common area. Initialize it */
        for (int i = 0; i < 256; i++)
            tStruct.intUnion.push_back(0x00);
    }
    for (auto n = node->child; n != NULL; n = n->nextSib) {
        vField = visit(n);
        if (n->astType.compare("FieldDecl") == 0) {
            tStruct.intStruct.push_back(vField);
            if (tStruct.typeEnum == Variable::typeEnum_t::isStruct) {
                tStruct.uData.uSize += vField.uData.uSize;
            }
            if (tStruct.typeEnum == Variable::typeEnum_t::isUnion) {
                // Make unione size the biggest of its memebrs
                if (tStruct.uData.uSize < vField.uData.uSize) {
                    tStruct.uData.uSize = vField.uData.uSize;
                }
            }
        }
    }
    if (smStructDefinition[2].compare("struct") == 0) {
        vStruct.push_back(tStruct);
    }
    else if (smStructDefinition[2].compare("union") == 0) {
        vUnion.push_back(tStruct);
    }
    return tStruct;
}
Variable fIndirectFieldDecl(Node* node) {
    return Variable();
}
Variable fMemberExpr(Node* node) {

    Variable v, ret;
    std::string memberName;
    std::smatch smMemberExpr;
    std::regex_search(node->text, smMemberExpr, eMemberExpr);
    memberName = smMemberExpr[3];
    v = visit(node->child);
    if (v.pointsTo->typeEnum == Variable::typeEnum_t::isStruct ||
        v.pointsTo->typeEnum == Variable::typeEnum_t::isUnion) {
        for (auto m = v.pointsTo->intStruct.begin(); m != v.pointsTo->intStruct.end(); m++) {
            if (memberName.compare(m->name) == 0) {
                ret.pointsTo = &*m;
                ret.typeEnum = Variable::typeEnum_t::isRef;
                // Always return a pointer because Memeber Expression is a lvalue
                return ret;
            }
        }
    }
    else {
        std::cout << "Error referencing variable is not a struct.";
    }
}
Variable fIfStmt(Node* node) {

    Variable cond, vtrue, vfalse;
    //cond = visit(node->child->nextSib->nextSib);  You may see this version <<NULL>>
    cond = visit(node->child);
    if (cond.value) {
        if (node->child->nextSib)
            //vtrue = visit(node->child->nextSib->nextSib->nextSib); You may see this version <<NULL>>
            vtrue = visit(node->child->nextSib);
    }
    else {
        if (node->child->nextSib->nextSib)
            //vfalse = visit(node->child->nextSib->nextSib->nextSib->nextSib); You may see this version <<NULL>>
            vfalse = visit(node->child->nextSib->nextSib);
    }
    return Variable();
}
Variable fForStmt(Node* node) {

    Node* init, * cond, * iter, * stmt;
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
Variable fCompoundStmt(Node* node) {
    Variable temp;
    Variable ret;
    VariableShadow::vVariable v;
    VariableShadow::vVariable* vVar;
    shadowLevel++;
    vShadowedVar.shadows.push_back(v);
    for (auto next = node->child; next != NULL; next = next->nextSib) {
        temp = visit(next);
    }
    vVar = &vShadowedVar.shadows.back();
    // For each variable that will go out of scope it searches if there are 
    // pointer that points to it and will set them to NULL.
    for (auto var = vVar->begin(); var != vVar->end(); var++) {
        for (auto it = vShadowedVar.shadows.rbegin(); it != vShadowedVar.shadows.rend(); it++) {
            for (auto itit = it->begin(); itit != it->end(); itit++) {
                recurseVariable(&*itit, &*var, nullifyPointer);
            }
        }
    }
    vShadowedVar.shadows.pop_back();
    shadowLevel--;
    return ret;
}
Variable fTranslationUnitDecl(Node* node) {
    Variable temp;
    for (auto next = node->child; next != NULL; next = next->nextSib) {
        temp = visit(next);
    }
    return Variable();
}
Variable fArraySubscriptExpr(Node* node) {
    Variable ret;
    Variable pArray = visit(node->child);
    int ix = visit(node->child->nextSib).value;
    ret.pointsTo = &pArray.pointsTo->array[ix];
    ret.typeEnum = Variable::typeEnum_t::isRef;
    return ret;
}
Variable fFunctionDecl(Node* node) {
    Variable temp;
    /* Make sure it's not a function declaration*/
    if (node->child) {
        temp = visit(node->child);
    for (auto it = vShadowedVar.shadows.rbegin(); it != vShadowedVar.shadows.rend(); it++) {
        for (auto itit = it->begin(); itit != it->end(); itit++) {
            itit->print();
        }
    }
    }
    return temp;
}
Variable fDeclStmt(Node* node) {
    Variable temp = visit(node->child);
    return temp;
}
Variable fNULL(Node* node) {
    return Variable();
}
Variable fFullComment(Node* node) {
    return Variable();
}
Variable fConstantExpr(Node* node) {
    /* As simple as that */
    return visit(node->child);
}


