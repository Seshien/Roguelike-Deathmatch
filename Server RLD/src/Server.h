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
#include "..\..\Utilities\Config.h"

enum class GameState { LOBBY, GAME_MID, GAME_END };
enum class StateChange { NONE, VOTE_END, GAME_END };


class Server
{
		
	public:
		
		GameState gameState;

		Server()
		{

		}
		void startServer();

	private:
		Parser::Messenger output;
		Parser::Messenger input;
		Parser::Messenger gameInput;
		std::shared_ptr<Config::ConfigHandler> conf;

		// void GenerateMap();
		void startMap();

		void startLobby();
		void mainLoop();
		void startGame();

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


		int getPlayerCount();
		int getPlayerCount(Player::State state);

		// TO DO
		std::string getResults();

		std::chrono::duration<double> getCurrentGameTime();

		std::vector<Player> playerList;
		std::vector<Player *> activePlayerList;
		std::vector<Player *> refreshActivePlayerList();
		Player * getPlayer(int playerID);

		int gameTickTimer = 0;
		Network network;
		Game game;

		int numOfVotes;
		int activePlayerCount;
		StateChange stateChange;
		std::string winner;

		std::chrono::system_clock::time_point gameStartTime;
		std::chrono::system_clock::time_point turntimer;
};

