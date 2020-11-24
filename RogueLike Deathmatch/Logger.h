#pragma once
#include <iostream>
#include <fstream>
#include <string>

class Logger
{
public:
	Logger()
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
				this->file.open("data/logger" + std::to_string(number) + ".txt");
				break;
			}
		}
	}

	~Logger()
	{
		file.close();
	}
	void log(std::string text)
	{
		std::cout << text << std::endl;
		file << text;
	}
private:
	std::ofstream file;
};

