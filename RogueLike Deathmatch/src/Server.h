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

			if (network.startServer(this->port))
			{
				Logger::log("Server network start failed. Closing server.");
				return;
			}
			startLobby();
			loopLobby();
			//startGame();
			//loopGame();
			return;
		}

	private:
		Parser::Messenger output;
		Parser::Messenger input;

		void startLogger();

		// void GenerateMap();
		void startMap();

		void startLobby()
		{
			//poczatek prawdziwej dzialalnosci serwera, nie wiem co tu dac zbytnio
		}
		void loopLobby();
		void startGame()
		{

		}
		void loopGame()
		{

		}
		void handleEvents(Parser::Messenger parser);

		void loadConfig();

		void processConfigLine(std::string line);

		void setConfigValue(std::string token, std::string value);
		std::string confName = "./data/config.txt";
		std::string mapPath = "./data/world0.txt";
		std::string port = "7777";
		Map map;
		Network network;

		int time;
};

