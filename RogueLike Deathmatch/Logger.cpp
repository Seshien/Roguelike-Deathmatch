#include "Logger.h"



namespace Logger
{
	std::ofstream file;
	bool _file = false;

	void startLogger()
	{
		int number = 0;
		std::ifstream check;
		while (true)
		{
			check.open("data/logger" + std::to_string(number) + ".txt");
			if (check.good())
			{
				number++;
				check.close();
			}
			else
			{
				check.close();
				file.open("data/logger" + std::to_string(number) + ".txt");
				break;
			}
		}
		_file = true;
	}

	void endLogger()
	{
		file.close();
	}
	void log(std::string text)
	{
		if (!_file)
		{
			std::cout << "Logger is not initialized" << std::endl;
			return;
		}
		std::cout << text << std::endl;
		file << text;
	}
	void log(std::string text, int error_code)
	{
		if (!_file)
		{
			std::cout << "Logger is not initialized" << std::endl;
			return;
		}
		file << text;
	}

	void logNetworkError() {
		wchar_t* s = NULL;
		FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPWSTR)&s, 0, NULL);
		fprintf(stderr, "%S\n", s);
		file << s << std::endl;
		LocalFree(s);
	}
};