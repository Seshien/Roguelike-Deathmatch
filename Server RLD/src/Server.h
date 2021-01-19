#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

#include "Map.h"
#include "Player.h"
#include "Game.h"
#include "..\..\Utilities\Event.h"
#include "..\..\Utilities\Parser.h"
#include "..\..\Utilities\Logger.h"
#include "..\..\Utilities\Network.h"
#include "..\..\Utilities\Constants.h"

class Server
{
	public:
		enum GameState { LOBBY, GAME_MID, GAME_END };
		enum class StateChange {NONE, VOTE_END, GAME_END};
		GameState gameState;

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
			mainLoop();
			//startGame();
			//loopGame();
			return;
		}

	private:
		Parser::Messenger output;
		Parser::Messenger input;
		Parser::Messenger gameInput;
		Parser::Messenger gameOutput;

		void startLogger();

		// void GenerateMap();
		void startMap();

		void startLobby()
		{
			//poczatek prawdziwej dzialalnosci serwera, nie wiem co tu dac zbytnio
			this->gameState = LOBBY;
		}
		void mainLoop();
		void startGame()
		{
			gameStartTime = std::chrono::system_clock::now();
		}
		void handleEvents(Parser::Messenger parser);

		void handleServer(Parser::Event ev);
		void handleLobby(Parser::Event ev);
		void handleGame(Parser::Event ev);
		void handleError(Parser::Event ev);

		void handleNewPlayer(Parser::Event ev);
		void handleTimeout(Parser::Event ev);
		void handleTimeoutAnswer(Parser::Event ev);
		void handleDisconnect(Parser::Event ev);
		void handleDisconnect(int playerID);
		void handleStateChange();

		void handleGameOutput(Parser::Messenger gameOutput);

		void InfoDump(int playerId);

		void handleVote(Parser::Event ev);

		void loadConfig();

		void processConfigLine(std::string line);

		int calcActivePlayerCount();

		int getPlayerCount();
		int getPlayerCount(Player::State state);

		// TO DO
		std::string getResults();

		std::chrono::duration<double> getCurrentGameTime();
		std::vector<Player *> getActivePlayerList();
		Player * getPlayer(int playerID);


		void setConfigValue(std::string token, std::string value);
		std::vector<Player> playerList;
		std::string confName = "./data/config.txt";
		std::string mapPath = "./data/world0.txt";
		std::string port = "7777";

		int mapID = 0;
		Network network;
		Game game;

		int time;
		int numOfVotes;
		int activePlayerCount;
		StateChange stateChange;

		std::chrono::system_clock::time_point gameStartTime;
		std::chrono::system_clock::time_point turntimer;
};

