#pragma once
#include <fstream>
#include <sstream>
#include "node.h"
class ValuesFile
{
	std::ofstream file;
	std::string fileName;
	Node* node;
	void recurseTree(Node* node);
	enum ValueEnum_t {
		isInteger,
		isFloat
	};

public:
	ValuesFile();
	ValuesFile(std::string name, Node *node);
	void writeFile();

};

