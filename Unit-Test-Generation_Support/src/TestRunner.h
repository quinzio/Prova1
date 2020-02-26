#pragma once
#include <string>

class TestRunner
{
public:
	bool buildGlobals;
	bool cleanSetByUser;
	bool freeRunning;
	std::string targetFunction;
	std::string inputValuesFile;
	std::string tempValuesFile;
	std::string expectedValuesFile;

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

