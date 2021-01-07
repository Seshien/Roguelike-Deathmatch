#include "Logger.h"



namespace Logger
{
	std::ofstream file;
	bool _file = false;

	void startLogger()
	{
		int number = 0;
		while (true)
		{
			std::string filename = "data/logger" + std::to_string(number) + ".txt";
			if (std::filesystem::exists(filename))
			{
				number++;
			}
			else
			{
				file.open(filename);
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
		file << text << std::flush;
	}
	void log(std::string text, int error_code)
	{
		if (!_file)
		{
			std::cout << "Logger is not initialized" << std::endl;
			return;
		}
		file << text << std::flush;
	}

	void logNetworkError() {
		int err = WSAGetLastError();
		wchar_t* s = NULL;
		FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, err,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPWSTR)&s, 0, NULL);
		std::cout << "Error: " << err << std::endl;
		fprintf(stderr, "%S\n", s);
		file << "Error: "<< err << std::endl;
		file << s << std::endl << std::flush;
		LocalFree(s);
	}
};