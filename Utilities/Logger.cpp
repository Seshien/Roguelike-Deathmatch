#include "Logger.h"



namespace Logger
{
	std::ofstream file;
	bool _file = false;
	int debug_mode = 0;

	void startLogger(std::string name)
	{
		int number = 0;
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

	void setLogLevel(int logLevel)
	{
		debug_mode = logLevel;
	}

	void endLogger()
	{
		file.close();
		_file = false;
	}


	void log(std::string text, std::ostream& ostr)
	{
		if (!_file)
		{
			std::cout << "Logger is not initialized" << std::endl;
			return;
		}
		ostr << text << std::endl << std::flush;
	}

	void log(std::string text, int value, std::ostream& ostr)
	{
		if (!_file)
		{
			std::cout << "Logger is not initialized" << std::endl;
			return;
		}
		ostr << text << " " << value << std::endl << std::flush;
	}

	void log(Parser::Event ev, std::ostream& ostr)
	{
		if (!_file)
		{
			std::cout << "Logger is not initialized" << std::endl;
			return;
		}

		ostr << "----------" << std::endl << "Event:" << std::endl <<
			"sender:receiver - " << ev.sender << " : " << ev.receiver << std::endl <<
			"type:subtype - " << Parser::convertToString(ev.type) << " : " << Parser::convertToString(ev.subtype) << std::endl <<
			"subdata - " << ev.subdata << std::endl << "----------" << std::endl << std::flush;
	}

	void error(std::string text)
	{
		log(text, std::cout);
		log(text, file);
	}

	void error(std::string text, int value)
	{
		log(text, value, std::cout);
		log(text, file);
	}

	void error(Parser::Event ev)
	{
		log(ev, std::cout);
		log(ev, file);
	}

	void debug(std::string text)
	{
		if (debug_mode >= 1)
			log(text, std::cout);
		log(text, file);
	}

	void debug(std::string text, int value)
	{
		if (debug_mode >= 1)
			log(text, value, std::cout);
		log(text, value, file);
	}

	void debug(Parser::Event ev)
	{
		if (debug_mode >= 1)
			log(ev, std::cout);
		log(ev, file);
	}

	void info(std::string text)
	{
		if (debug_mode >= 0)
			log(text, std::cout);
		log(text, file);
	}

	void info(std::string text, int value)
	{
		if (debug_mode >= 0)
			log(text, value, std::cout);
		log(text, value, file);
	}

	void info(Parser::Event ev)
	{
		if (debug_mode >= 0)
			log(ev, std::cout);
		log(ev, file);
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
