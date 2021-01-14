#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <winsock2.h>

namespace Logger
{
	void startLogger();

	void endLogger();
	void log(std::string text);
	void log(std::string text, int error_code);
	void log(int receiver, int sender, int type, int subtype, std::string subdata);
	void logNetworkError();

};

