#pragma once
#include <iostream>
#include <fstream>
#include <string>

#include "Map.h"
#include "Logger.h"
#include "Network.h"
#include "Parser.h"
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
			network.startNetwork(this->port);
			startLobby();
			loopLobby();
			//startGame();
			//loopGame();
			return;
		}

	private:
		Parser output;
		Parser input;

		void startLogger()
		{
			Logger::startLogger();
		}

		// void GenerateMap();
		void startMap()
		{
			this->map.init(mapPath);
		}

		void startLobby()
		{
			//poczatek prawdziwej dzialalnosci serwera, nie wiem co tu dac zbytnio
		}
		void loopLobby()
		{
			while (true)
			{
				input = this->network.inputNetwork();
				for (auto ev : input.eventList)
					handleEvent(ev);
				for (auto ev : output.eventList) {
					std::cout << "x" << std::endl;
				}

			}

		}
		void startGame()
		{

		}
		void loopGame()
		{

		}
		void handleEvent(Parser::Event ev)
		{
			//zaleznie od typu eventu cos by trzeba bylo zrobic 
		}

		void loadConfig()
		{
			std::ifstream file;
			std::string line;
			file.open(this->confName);
			if (file.is_open())
			{
				Logger::log("Config file opened:");
				while (std::getline(file, line))
				{
					Logger::log(line);
					this->processConfigLine(line);
				}
			}

			else Logger::log("Config file not found");
			file.close();
		}

		void processConfigLine(std::string line)
		{
			std::string delimiter = ":";
			int pos = line.find(delimiter);
			if (pos == -1)
			{
				Logger::log("Error during parsing of config file");
				return;
			}
			std::string token = line.substr(0, pos);
			std::string value = line.substr(pos + 1, line.length() - 1);
			Logger::log("Token: " + token + " Value: " + value);
			this->setConfigValue(token, value);
			return;
		}

		void setConfigValue(std::string token, std::string value)
		{
			if (token == "time") this->time = std::stoi(value);
			else if (token == "special") return;
			else if (token == "port") this->port = value;
		}
		std::string confName = "data/config.txt";
		std::string mapPath = "data/world0.txt";
		std::string port = "7777";
		Map map;
		Network network;

		int time;
};

