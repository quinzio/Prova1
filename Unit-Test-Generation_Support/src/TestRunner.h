#pragma once
#include <string>

class TestRunner
{
public:
	bool buildGlobals;
	bool cleanSetByUser;
	bool freeRunning;
	std::string targetFunction;
	std::string neutralValuesFile;
	std::string startValuesFile;
	std::string inputValuesFile;
	std::string expectedValuesFile;
	std::string callsFile;
	std::string testFile;

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

