#include <string>
#include <iostream>
#include "Variabile.h"
#include "Unit-Test-Generation-Support.h"

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
	if ((long long)this->value < 0) {
		res = prefix + this->name + postfix + " = " + std::to_string((long long)this->value) + "\n";
	}
	else {
		res = prefix + this->name + postfix + " = " + std::to_string(this->value) + "\n";
	}
	std::cout << res;
	Variable::outputFile << res;
	int ix = 0;
	if (this->typeEnum == Variable::typeEnum_t::isArray) {
		for (auto v = this->array.begin(); v != this->array.end(); v++) {
			tempPrefix = prefix + this->name + "[" + std::to_string(ix) + "]";
			tempPostfix = postfix;
			v->print(tempPrefix, tempPostfix);
			ix++;
		}
	}
	if (this->typeEnum == Variable::typeEnum_t::isStruct) {
		for (auto v = this->intStruct.begin(); v != this->intStruct.end(); v++) {
			tempPrefix = prefix + this->name + ".";
			tempPostfix = postfix;
			v->print(tempPrefix, tempPostfix);
		}
	}
	if (this->typeEnum == Variable::typeEnum_t::isUnion) {
		for (auto v = this->intStruct.begin(); v != this->intStruct.end(); v++) {
			tempPrefix = prefix + this->name + ".";
			tempPostfix = postfix;
			v->print(tempPrefix, tempPostfix);
		}
	}
	if (this->typeEnum == Variable::typeEnum_t::isRef) {
		if (this->pointsTo) {
			tempPrefix = prefix + this->name + "->";
			tempPostfix = postfix;
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

void Variable::updateCommonArea(void) {
	if (this->uData.myParent && 
		this->uData.myParent->typeEnum == Variable::typeEnum_t::isUnion) 
	{
		int bbOffset_byte = this->uData.uOffset.getBytes();
		int bbSize_byte = this->uData.uSize.getBytes();
		int bbSize_bit = this->uData.uSize.getBits();
		/* In case that we have to copy Bytes (not Bits)
		*/
		if (bbSize_bit) {

		}
		else {
			for (int i = 0; i < bbSize_byte; i++) {
				this->uData.myParent->intUnion.at(bbOffset_byte++) =
					((unsigned char*)&this->value)[i];
			}
		}
		/* Now update all the members */
		this->uData.myParent->updateUnion();
	}
}

void Variable::updateUnion() {
	if (this->typeEnum == Variable::typeEnum_t::isArray) {
		for (auto it = this->array.begin(); it != this->array.end(); it++) {
			(*it).updateUnion();
		}
	}
	else if (this->typeEnum == Variable::typeEnum_t::isStruct) {
		for (auto it = this->intStruct.begin(); it != this->intStruct.end(); it++) {
			(*it).updateUnion();
		}
	}
	else if (this->typeEnum == Variable::typeEnum_t::isUnion) {
		for (auto it = this->intStruct.begin(); it != this->intStruct.end(); it++) {
			(*it).updateUnion();
		}
	}
	else {
		Variable* p; // parent
		unsigned long long value = 0;
		p = this->uData.myParent;
		int sizeByte = this->uData.uSize.getBytes();
		int sizeBit = this->uData.uSize.getBits();
		int offsetByte = this->uData.uOffset.getBytes();
		int offsetBit = this->uData.uOffset.getBits();
		if (sizeBit) {
			// bits !!!
		}
		else {
			for (int i = 0; i < sizeByte; i++) {
				*((unsigned char*)&value + i) =
					p->intUnion.at(i + offsetByte);
			}
			signExtend(this);
		}
		if (this->typeEnum == Variable::typeEnum_t::isRef) {
			this->pointsTo = (Variable*)value;
		}
		else {
			this->value = value;
		}
	}
	return;
}

