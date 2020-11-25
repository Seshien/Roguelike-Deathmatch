#pragma once
#include <iostream>
#include <fstream>
#include <string>

namespace Logger
{

	void startLogger();

	void endLogger();
	void log(std::string text);
	void log(std::string text, int error_code);

};

