#pragma once
#include <iostream>
#include <string>
#include <regex>
#include "Variabile.h"
#include "Node.h"
#include "Unit-Test-Generation-Support.h"

int inner_main(int argc, std::string argv[]) throw (const std::exception&);

void createBuiltInTypes(void);
Variable& valueCast(std::string str, Variable& v);
void recurseVariable(Variable* v, Variable* ref, void (*fp)(Variable*, Variable*));
void nullifyPointer(Variable* v, Variable* ref);
void cleanTestStorage();
void expThrow(std::string mex);
void setSourceLocations(Node* tempNode, std::string str);
void setGlobalLocation(std::smatch smSourcePoint);
Variable buildVariable(struct coreType_str& CoreTypes, Node* node = NULL);
BbSize setVariableOffset(Variable* v, BbSize vOffset = BbSize());
void updateUnion(Variable* v);
void signExtend(Variable* v);
void careUnions(Variable* v);
Variable visit(Node* node);
Variable getTypDef(std::string uType);
Variable fVarDecl(Node* node);
Variable fIntegerLiteral(Node* node);
Variable fFloatingLiteral(Node* node);
Variable fTypedefDecl(Node* node);
Variable fDeclRefExpr(Node* node);
Variable fImplicitCastExpr(Node* node);
Variable fUnaryOperator(Node* node);
Variable fBinaryOperator(Node* node);
Variable fFieldDecl(Node* node);
Variable fRecordDecl(Node* node);
Variable fIndirectFieldDecl(Node* node);
Variable fMemberExpr(Node* node);
Variable fIfStmt(Node* node);
Variable fForStmt(Node* node);
Variable fCompoundStmt(Node* node);
Variable fTranslationUnitDecl(Node* node);
Variable fArraySubscriptExpr(Node* node);
Variable fFunctionDecl(Node* node);
Variable fDeclStmt(Node* node);
Variable fNULL(Node* node);
Variable fFullComment(Node* node);
Variable fConstantExpr(Node* node);
Variable fEnumDecl(Node* node);
Variable fEnumConstantDecl(Node* node);
Variable fParmVarDecl(Node* node);
Variable fCallExpr(Node* node);
Variable fParenExpr(Node* node);
Variable fReturnStmt(Node* node);
Variable fCStyleCastExprt(Node* node);
Variable fCompoundAssignOperator(Node* node);
Variable fWhileStmt(Node* node);
Variable fConditionalOperator(Node* node);


