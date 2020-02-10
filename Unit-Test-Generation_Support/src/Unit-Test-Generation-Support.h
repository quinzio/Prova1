#pragma once
#include <iostream>
#include <string>
#include <regex>
#include "Variabile.h"
#include "Node.h"
#include "Unit-Test-Generation-Support.h"

int inner_main(int argc, std::string argv[]);
void recurseVariable(Variable* v, Variable* ref, void (*fp)(Variable* , Variable*));
void nullifyPointer(Variable* v, Variable* ref);
void cleanTestStorage();
void setSourceLocations(Node* tempNode, std::string str);
void setGlobalLocation(std::smatch smSourcePoint);
Variable buildVariable(struct coreType_str& CoreTypes, Node* node = NULL);
BbSize setVariableOffset(Variable* v, BbSize vOffset = BbSize());
Variable getTypDef(std::string uType);
Variable fVarDecl(Node* node);
Variable fIntegerLiteral(Node* node);
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
