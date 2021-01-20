#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <winsock2.h>

#include "Event.h"
#include"Constants.h"

namespace Logger
{
	void startLogger(std::string name, bool mode = Constants::debug);

	void endLogger();

	void error(std::string text);
	void error(std::string text, int value);
	void error(Parser::Event ev);

	void debug(std::string text);
	void debug(std::string text, int value);
	void debug(Parser::Event ev);

	void log(std::string text);
	void log(std::string text, int error_code);
	void log(Parser::Event ev);
	void logNetworkError();

};

