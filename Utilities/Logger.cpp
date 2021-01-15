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
	void log(int sender, int receiver, int type, int subtype, std::string subdata)
	{
		if (!_file)
		{
			std::cout << "Logger is not initialized" << std::endl;
			return;
		}
		std::cout << "Event:" << std::endl << 
			"sender:receiver - " << sender << " : " << receiver << std::endl <<
			"type:subtype - " << type << " : " << subtype << std::endl <<
			"subdata - " << subdata << std::endl << std::flush;

		file << "Event:" << std::endl <<
			"sender:receiver - " << sender << " : " << receiver << std::endl <<
			"type:subtype - " << type << " : " << subtype << std::endl <<
			"subdata - " << subdata << std::endl << std::flush;
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