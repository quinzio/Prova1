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

#include "Unit-Test-Generation-Support.h"
#include "Variabile.h"
#include "Node.h"
#include "CoreTypes.h"
#include "RegexColl.h"


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
    if (myRoot.child) 
        t = visit(myRoot.child);
     infile.close();
     Variable::outputFile.close();
    return 0;
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
void recurseVaribale(Variable* v,  Variable* ref, void (*fp)(Variable* , Variable* )) {
    if (v->typeEnum == Variable::typeEnum_t::isRef) {
        fp(v, ref);
    }
    if (v->typeEnum == Variable::typeEnum_t::isArray) {
        for (auto it = v->array.begin(); it != v->array.end(); it++) {
            recurseVaribale(&*it, ref, fp);
        }
    }
    if (v->typeEnum == Variable::typeEnum_t::isStruct) {
        for (auto it = v->intStruct.begin(); it != v->intStruct.end(); it++) {
            recurseVaribale(&*it, ref, fp);
        }
    }
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

    temp = buildVariable(CoreTypes);
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
    return temp;
}
Variable fIntegerLiteral(Node* node) {
    unsigned long long integral;
    Variable ret;
    std::smatch smIntegralType;
    std::regex_search(node->text, smIntegralType, eIntegralType);
    integral = std::stoi(smIntegralType[2]);
    ret.value = cast(smIntegralType[1], integral);
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
    if (smImplicitCastExpr[2].compare("LValueToRValue") == 0) {
        if (ret.typeEnum == Variable::typeEnum_t::isArray) {
            return ret.pointsTo->array[ret.pointsTo->arrayIx];
        }
        return *ret.pointsTo;
    }
    // No cast should be necessary as the type is not changed by 
    // ImplicitCastExpr
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
        *opa.pointsTo = opb;
        opa.pointsTo->name = saveName;
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
    std::smatch smStructType;
    std::string rawType;
    std::string name;
    std::string referenced;
    std::string refinedType = "";
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
        refinedType = smStructTypeAnonymous[1];
    }
    else if (std::regex_search(rawType, smStructTypeAnonymous2, eStructTypeAnonymous2)) {
        refinedType = smStructTypeAnonymous2[1];
    }
    else if (std::regex_search(rawType, smStructType, eStructType)) {
        refinedType = smStructType[1];
    }
    if (refinedType.length() > 0)
    {
        // Field is a nested struct
        for (Variable v : vStruct) {
            if (refinedType.compare(v.name) == 0) {
                vField = v;
                break;
            }
        }
    }
    else {
        vField.typeEnum = Variable::typeEnum_t::isValue;
        vField.type = rawType;
    }
    vField.name = name;
    // Note the blank space in " referenced"
    if (smFieldDeclaration[1].compare(" referenced") == 0) {
        vField.used = true;
    }
    return vField;
}
Variable fRecordDecl(Node* node) {
    Variable tStruct, vField;
    std::smatch smStructDefinition;
    std::regex_search(node->text, smStructDefinition, eStructDefinition);
    // Anonimous structures will take name as file.ext:line:col
    tStruct.name = smStructDefinition[2];
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
    tStruct.typeEnum = Variable::typeEnum_t::isStruct;
    for (auto n = node->child; n != NULL; n = n->nextSib) {
        vField = visit(n);
        if (n->astType.compare("FieldDecl") == 0) {
            tStruct.intStruct.push_back(vField);
        }
    }
    vStruct.push_back(tStruct);
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
    memberName = smMemberExpr[2];
    v = visit(node->child);
    if (v.pointsTo->typeEnum == Variable::typeEnum_t::isStruct) {
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
                recurseVaribale(&*itit, &*var, nullifyPointer);
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
    Variable temp = visit(node->child);
    for (auto it = vShadowedVar.shadows.rbegin(); it != vShadowedVar.shadows.rend(); it++) {
        for (auto itit = it->begin(); itit != it->end(); itit++) {
            itit->print();
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
Variable buildVariable(struct coreType_str& CoreTypes, Node *node) {
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
        node = node->child;
        while (node) {
            std::smatch smTemp;
            if (std::regex_search(node->text, smTemp, std::regex(
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
            }
            while (node->nextSib) {

                node = node->nextSib;
            }
            node = node->child;
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
            temp = vIt;
        }
        else if (std::regex_search(CoreTypes.typeChainA.at(i), smTemp, std::regex("\\*"))) {
            /* A pointer
               so the final type will be this pointer and not the previously found
               finalType. Reinitialize temp */
            temp = Variable();
            temp.typeEnum = Variable::typeEnum_t::isRef;
            temp.value = 0;
        }
    }
    return temp;
}
