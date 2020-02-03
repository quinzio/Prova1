#pragma once
#include <iostream>
#include "Variabile.h"
#include "Unit-Test-Generation-Support.h"
int inner_main(int argc, std::string argv[]);
void recurseVaribale(Variable* v, Variable* ref, void (*fp)(Variable* , Variable*));
void nullifyPointer(Variable* v, Variable* ref);
void cleanTestStorage();