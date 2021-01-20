#include "Logger.h"



namespace Logger
{
	std::ofstream file;
	bool _file = false;
	bool debug_mode = false;

	void startLogger(std::string name, bool mode)
	{
		int number = 0;
		debug_mode = mode;
		while (true)
		{
			if (!std::filesystem::is_directory("log")) 
				std::filesystem::create_directory("log");
			std::string filename = "log/" + name + std::to_string(number) + ".txt";
			if (std::filesystem::exists(filename))
			{
				number++;
			}
			else
			{
				file.open(filename);
				file << "This is " << name << " log file" << std::endl << std::flush;
				break;
			}
		}
		_file = true;
	}

	void endLogger()
	{
		file.close();
	}

	void error(std::string text)
	{
		log(text);
	}

	void error(std::string text, int value)
	{
		log(text, value);
	}

	void error(Parser::Event ev)
	{
		log(ev);
	}

	void debug(std::string text)
	{
		if (debug_mode) 
			log(text);
	}

	void debug(std::string text, int value)
	{
		if (debug_mode)
			log(text, value);
	}

	void debug(Parser::Event ev)
	{
		if (debug_mode)
			log(ev);
	}

	void log(std::string text)
	{
		if (!_file)
		{
			std::cout << "Logger is not initialized" << std::endl;
			return;
		}
		std::cout << text << std::endl << std::flush;
		file << text << std::endl << std::flush;
	}

	void log(std::string text, int value)
	{
		if (!_file)
		{
			std::cout << "Logger is not initialized" << std::endl;
			return;
		}
		std::cout << text << " " << value << std::endl << std::flush;
		file << text << " " << value << std::endl << std::flush;
	}

	void log(Parser::Event ev)
	{
		if (!_file)
		{
			std::cout << "Logger is not initialized" << std::endl;
			return;
		}
		
		std::cout << "----------" << std::endl << "Event:" << std::endl <<
			"sender:receiver - " << ev.sender << " : " << ev.receiver << std::endl <<
			"type:subtype - " << Parser::convertToString(ev.type) << " : " << Parser::convertToString(ev.subtype) << std::endl <<
			"subdata - " << ev.subdata << std::endl << "----------" << std::endl << std::flush;

		file << "----------" << std::endl << "Event:" << std::endl <<
			"sender:receiver - " << ev.sender << " : " << ev.receiver << std::endl <<
			"type:subtype - " << Parser::convertToString(ev.type) << " : " << Parser::convertToString(ev.subtype) << std::endl <<
			"subdata - " << ev.subdata << std::endl << "----------" << std::endl << std::flush;
	}

	void logNetworkError() {
		int err = WSAGetLastError();

	//	LPSTR s = nullptr;
	//	int size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	//		NULL, err,
	//		0,
	//		(LPWSTR)&s, 0, NULL);

		std::cout << "Error: " << err << std::endl << std::endl << std::flush;
		file << "Error: "<< err << std::endl << std::endl << std::flush;
	//	LocalFree(s);
	}
};
