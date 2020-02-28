#include "Node.h"

Node::Node()
{
    depth = 0;
    type = Node::ValueEnum_t::isNothing;
}

Node::~Node()
{
}

void Node::setValue(unsigned long long value)
{
    this->value = value;
    type = Node::ValueEnum_t::isInteger;
}

unsigned long long Node::getValue()
{
    return value;
}

void Node::setValueDouble(float value)
{
    this->valueDouble = value;
    type = Node::ValueEnum_t::isFloat;
}

float Node::getValueDouble()
{
    return valueDouble;
}

Node::ValueEnum_t Node::get_type()
{
    return type;
}

bool Node::getCallExprEvaluated()
{
	return callExprEvaluated;
}

void Node::setCallExprEvaluated(bool val)
{
    callExprEvaluated = val;
}
