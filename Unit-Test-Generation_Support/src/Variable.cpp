#include <string>
#include <iostream>
#include "Variabile.h"


std::string Variable::print() {
	std::string prefix = "";
	std::string postfix = "";
	this->print(prefix, postfix);
	return "";
}

std:: string Variable::print(std::string prefix, std::string postfix) {
	std::string tempPrefix;
	std::string tempPostfix;
	std::string res;
	res = prefix + this->name + postfix + " = " + std::to_string(this->value) + "\n";
	std::cout << res;
	Variable::outputFile << res;
	int ix = 0;
	if (this->typeEnum == Variable::typeEnum_t::isArray) {
		for (auto v = this->array.begin(); v != this->array.end(); v++) {
			tempPrefix = prefix;
			tempPostfix =  "[" + std::to_string(ix) + "\]" + postfix;
			v->print(tempPrefix, tempPostfix);
			ix++;
		}
	}
	if (this->typeEnum == Variable::typeEnum_t::isStruct) {
		for (auto v = this->intStruct.begin(); v != this->intStruct.end(); v++) {
			tempPrefix = prefix + "member " + v->name + " ";
			tempPostfix = "." + v->name + postfix;
			v->print(tempPrefix, tempPostfix);
		}
	}
	if (this->typeEnum == Variable::typeEnum_t::isRef) {
		if (this->pointsTo) {
			tempPrefix = prefix + this->name + postfix + " -> ";
			this->pointsTo->print(tempPrefix, postfix);
		}
		else {
			std::cout << "Null pointer\n";
			Variable::outputFile << "Null pointer\n";
		}
	}
	prefix = "";
	return prefix;
}