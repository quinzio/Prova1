#include <string>
#include <iostream>
#include "Variabile.h"
#include "Unit-Test-Generation-Support.h"

std::string Variable::print(bool replaceWSetByUser) {
	std::string prefix = "";
	std::string postfix = "";
	this->print(prefix, postfix, replaceWSetByUser);
	return "";
}

std::string Variable::print(std::string prefix, std::string postfix, bool replaceWSetByUser) {
	return Variable::print2(prefix, this->name, postfix, replaceWSetByUser);
}


std::string Variable::print2(std::string prefix, std::string nameComposite, std::string postfix, bool replaceWSetByUser) {
	std::string tempPrefix;
	std::string tempPostfix;
	std::string res;
	unsigned long long localValue;
	bool unsignedType = false;
	if (std::regex_search(this->type, std::regex("(.*)unsigned"))) {
		unsignedType = true;
	}
	if (replaceWSetByUser) {
		if (this->setByUser) {
			localValue = this->valueByUser;
		}
		else
		{
			localValue = this->valueDefault;
		}
	}
	else {
		localValue = this->value;
	}
	if (this->uData.uSize.getBytes() == 4) {
		if (unsignedType) {
			res = prefix + nameComposite + " = " + std::to_string((uint32_t)localValue) + postfix + "\n";
		}
		else {
			res = prefix + nameComposite + " = " + std::to_string((int32_t)localValue) + postfix + "\n";
		}
	}
	else if (this->uData.uSize.getBytes() == 2) {
		if (unsignedType) {
			res = prefix + nameComposite +  " = " + std::to_string((uint16_t)localValue) + postfix + "\n";
		}
		else {
			res = prefix + nameComposite + " = " + std::to_string((int16_t)localValue) + postfix + "\n";
		}
	}
	else if (this->uData.uSize.getBytes() == 1) {
		if (unsignedType) {
			res = prefix + nameComposite + " = " + std::to_string((uint8_t)localValue) + postfix + "\n";
		}
		else {
			res = prefix + nameComposite + " = " + std::to_string((int8_t)localValue) + postfix + "\n";
		}
	}
	else {
		res = prefix + nameComposite + " = " + std::to_string((uint32_t)localValue) + postfix + "\n";
	}
	if (this->usedInTest == true) {
		if (this->typeEnum == Variable::typeEnum_t::isValue) {
			std::cout << res;
			Variable::outputFile << res;
		}
	}
	int ix = 0;
	if (this->typeEnum == Variable::typeEnum_t::isArray) {
		int lastDiff = 0;
		for (auto v = this->array.begin(); v != this->array.end(); v++) {
			/* This will prevent writing a lot of equal elemt arrays 
			Take care of float and integer values to compare. 
			Take care of arrays of structures, unions, etc...
			*/
			if (ix > 0 && 
				this->array[0].typeEnum == Variable::typeEnum_t::isValue &&
				this->array[lastDiff].value == this->array[ix].value &&
				this->array[lastDiff].valueDouble == this->array[ix].valueDouble
				) {
			}
			else {
				lastDiff = ix;
				tempPrefix = prefix;
				nameComposite += "[" + std::to_string(ix) + "]";
				tempPostfix = postfix;
				v->print2(tempPrefix, nameComposite, tempPostfix, replaceWSetByUser);
			}
			ix++;
		}
	}
	if (this->typeEnum == Variable::typeEnum_t::isStruct) {
		for (auto v = this->intStruct.begin(); v != this->intStruct.end(); v++) {
			std::string newNameComposite;
			tempPrefix = prefix;
			newNameComposite = nameComposite + "." + v->name;
			tempPostfix = postfix;
			v->print2(tempPrefix, newNameComposite, tempPostfix, replaceWSetByUser);
		}
	}
	if (this->typeEnum == Variable::typeEnum_t::isUnion) {
		for (auto v = this->intStruct.begin(); v != this->intStruct.end(); v++) {
			std::string newNameComposite;
			tempPrefix = prefix;
			newNameComposite = nameComposite = "." + v->name;
			tempPostfix = postfix;
			v->print2(tempPrefix, newNameComposite, tempPostfix, replaceWSetByUser);
		}
	}
	if (this->typeEnum == Variable::typeEnum_t::isRef) {
		if (this->pointsTo) {
			tempPrefix = prefix;
			nameComposite = "(*" + nameComposite + ")";
			tempPostfix = postfix;
			this->pointsTo->print2(tempPrefix, nameComposite, tempPostfix, replaceWSetByUser);
		}
		else {
			std::cerr << "Null pointer\n";
			//Variable::outputFile << "Null pointer\n";
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
		int bbOffset_bit = this->uData.uOffset.getBits();
		int bbSize_byte = this->uData.uSize.getBytes();
		int bbSize_bit = this->uData.uSize.getBits();
		/* In case that we have to copy Bytes (not Bits)
		*/
		if (bbSize_bit) {
			/* Copy verbatim bit by bit */
			int length = bbSize_byte * 8 + bbSize_bit;
			BbSize from;
			BbSize to =  this->uData.uOffset;
			BbSize inc = BbSize(1, 0);
			bool val;
			unsigned char mask8;
			for (int ix = 0; ix < length; ix++) {
				val = ((unsigned char*)&this->value)[from.getBytes()] & (1 << from.getBits());
				mask8 = 1 << to.getBits();
				this->uData.myParent->intUnion.at(to.getBytes()) &= (~mask8);
				if (val) {
					this->uData.myParent->intUnion.at(to.getBytes()) |= mask8;
				}
				from = from + inc;
				to = to + inc;
			}
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
		int bbOffset_byte = this->uData.uOffset.getBytes();
		int bbOffset_bit = this->uData.uOffset.getBits();
		int bbSize_byte = this->uData.uSize.getBytes();
		int bbSize_bit = this->uData.uSize.getBits();
		if (bbSize_bit) {
			/* Copy verbatim bit by bit */
			int length = bbSize_byte * 8 + bbSize_bit;
			BbSize from = this->uData.uOffset;
			BbSize to;
			BbSize inc = BbSize(0,1);
			bool val;
			unsigned char mask8;
			for (int ix = 0; ix < length; ix++) {
				val = this->uData.myParent->intUnion.at(from.getBytes()) & (1 << from.getBits());
				mask8 = 1 << to.getBits();
				((unsigned char*)&this->value)[to.getBytes()] &= (~mask8);
				if (val) {
					((unsigned char*)&this->value)[to.getBytes()] |= mask8;
				}
				from = from + inc;
				to = to + inc;
			}
		}
		else {
			for (int i = 0; i < bbSize_byte; i++) {
				*((unsigned char*)&this->value + i) =
					p->intUnion.at(i + bbOffset_byte);
			}
			signExtend(this);
		}
		if (this->typeEnum == Variable::typeEnum_t::isRef) {
			this->pointsTo = (Variable*)value;
		}
	}
	return;
}

