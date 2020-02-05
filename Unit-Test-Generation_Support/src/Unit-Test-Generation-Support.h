#pragma once
#include <iostream>
#include <string>
#include <regex>
#include "Variabile.h"
#include "Node.h"
#include "Unit-Test-Generation-Support.h"


int inner_main(int argc, std::string argv[]);
void recurseVaribale(Variable* v, Variable* ref, void (*fp)(Variable* , Variable*));
void nullifyPointer(Variable* v, Variable* ref);
void cleanTestStorage();
void setSourceLocations(Node* tempNode, std::string str);
void setGlobalLocation(std::smatch smSourcePoint);
Variable getTypDef(std::string uType);
