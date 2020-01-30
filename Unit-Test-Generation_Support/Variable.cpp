#include <string>
#include <iostream>
#include "Variabile.h"


Variable::Variable()
{
}

Variable::~Variable()
{
}

std::string Variable::print() {
	std::string prefix = "";
	this->print(prefix);
	return "";
}

std:: string Variable::print(std::string prefix) {
	std::cout << prefix << "Value of variable " + this->name + ": " + std::to_string(this->value) + "\n";
	int ix = 0;
	if (this->typeEnum == Variable::typeEnum_t::isArray) {
		for (auto v = this->array.begin(); v != this->array.end(); v++) {
			std::cout << "array " << ix++ << " ";
			v->print();
		}
	}
	prefix += " struct";
	if (this->typeEnum == Variable::typeEnum_t::isStruct) {
			for (auto v = this->intStruct.begin(); v != this->intStruct.end(); v++) {
			v->print(prefix);
		}
	}
	prefix = "";
	return prefix;
}