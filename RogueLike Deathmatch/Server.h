#pragma once
#include <iostream>
#include <fstream>
#include <string>

#include "Map.h"
#include "Logger.h"

class Server
{

	public:

		Server()
		{

		}
		void StartServer()
		{
			startLogger();
			loadConfig();
			startMap();
			return;
			//StartNetwork();
			//StartLobby();
			//LoopLobby();
			//StartGame();
			//LoopGame();

		}

	private:
		void startLogger()
		{
		//this->logger = Logger()
		}

		void startNetwork();

		// void GenerateMap();
		void startMap()
		{
			this->map.init(mapPath);
		}

		void startLobby();
		void loopLobby();
		void startGame();
		void loopGame();

		void loadConfig()
		{
			std::ifstream file;
			std::string line;
			file.open(this->confName);
			if (file.is_open())
			{
				this->logger.log("Config file opened:");
				while (std::getline(file, line))
				{
					this->logger.log(line);
					this->processConfigLine(line);
				}
			}

			else this->logger.log("Config file not found");
			file.close();
		}

		void processConfigLine(std::string line)
		{
			std::string delimiter = ":";
			int pos = line.find(delimiter);
			if (pos == -1)
			{
				this->logger.log("Error during parsing of config file");
				return;
			}
			std::string token = line.substr(0, pos);
			std::string value = line.substr(pos + 1, line.length() - 1);
			this->logger.log("Token: " + token + " Value: " + value);
			this->setConfigValue(token, value);
			return;
		}

		void setConfigValue(std::string token, std::string value)
		{
			if (token == "time") this->time = std::stoi(value);
			else if (token == "special") return;
		}
		Logger logger;
		std::string confName = "data/config.txt";
		std::string mapPath = "data/world0.txt";
		Map map;


		int time;
};

