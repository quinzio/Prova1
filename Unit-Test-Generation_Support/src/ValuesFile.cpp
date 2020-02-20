#include <sstream>
#include <fstream>
#include <iomanip>
#include "ValuesFile.h"
#include "Unit-Test-Generation-Support.h"

ValuesFile::ValuesFile(std::string name, Node *node)
{
    fileName = name;
    file.open(fileName, std::fstream::in | std::fstream::out | std::fstream::trunc);
    if (!file.good())  {
        expThrow("Can not open Values File: " + fileName + "\n");
    }
    this->node = node;
}
ValuesFile::ValuesFile() {};

void ValuesFile::writeFile()
{
    if (!file.is_open()) {
        file.open(fileName, std::fstream::in | std::fstream::out | std::fstream::trunc);
        if (!file.good()) {
            expThrow("Can not open Values File: " + fileName + "\n");
        }
    }
    recurseTree(node);
    file.close();
}

void ValuesFile::recurseTree(Node* node)
{
    std::stringstream line;
    if (node->get_type() == Node::ValueEnum_t::isInteger) {
        line << std::setw(6) << (signed long long)node->getValue() << std::setw(1) << "  ";
    }
    else if (node->get_type() == Node::ValueEnum_t::isFloat) {
        line << std::setw(6) << node->getValueDouble() << std::setw(1) << "f ";
    }
    else {
        line << std::setw(6) << "---" << "  ";
    }
    //line << std::setw(10) << node->text << std::endl;
    line << std::endl;
    file << line.str();
    if (node->child) {
        recurseTree(node->child);
    }

    if (node->nextSib) {
        recurseTree(node->nextSib);
    }

}

