#pragma once
#include <string>

class TestRunner
{
public:
	bool buildGlobals;
	bool cleanSetByUser;
	bool freeRunning;
	std::string targetFunction;

	enum class TestState_enum {
		Init,
		BeginOfFunction,
		BuildVariable,
		FreeRun, 
		LockedRun, 
		Finish
	} testState;

	TestRunner();
};

