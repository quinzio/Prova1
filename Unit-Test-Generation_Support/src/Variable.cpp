#include <string>
#include <iostream>
#include "Variabile.h"


std::string Variable::print() {
	std::string prefix = "";
	this->print(prefix);
	return "";
}

std:: string Variable::print(std::string prefix) {
	std::string tempPrefix;
	std::cout << prefix << "Value of variable " + this->name + ": " + std::to_string(this->value) + "\n";
	int ix = 0;
	if (this->typeEnum == Variable::typeEnum_t::isArray) {
		for (auto v = this->array.begin(); v != this->array.end(); v++) {
			tempPrefix = prefix + "array " + std::to_string(ix) + " ";
			v->print(tempPrefix);
			ix++;
		}
	}
	if (this->typeEnum == Variable::typeEnum_t::isStruct) {
		for (auto v = this->intStruct.begin(); v != this->intStruct.end(); v++) {
			tempPrefix = prefix + "member " + v->name + " ";
			v->print(tempPrefix);
		}
	}
	if (this->typeEnum == Variable::typeEnum_t::isRef) {
		if (this->pointsTo) {
			prefix = prefix + "deref ";
			this->pointsTo->print(prefix);
		}
		else {
			std::cout << "Null pointer\n";
		}
	}
	prefix = "";
	return prefix;
}