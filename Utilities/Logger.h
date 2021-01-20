#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <winsock2.h>

#include "Event.h"

namespace Logger
{
	void startLogger(std::string name);

	void endLogger();
	void log(std::string text);
	void log(std::string text, int error_code);
	void log(Parser::Event ev);
	void logNetworkError();

};

