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

	void setLogLevel(int logLevel);

	void endLogger();

	void error(std::string text);
	void error(std::string text, int value);
	void error(Parser::Event ev);

	void info(std::string text);

	void info(std::string text, int value);

	void info(Parser::Event ev);

	void debug(std::string text);
	void debug(std::string text, int value);
	void debug(Parser::Event ev);

	void logNetworkError();

};

