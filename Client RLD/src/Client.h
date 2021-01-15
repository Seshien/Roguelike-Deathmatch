#pragma once
#include <string>
#include "..\..\Utilities\Event.h"
#include "..\..\Utilities\Network.h"
#include "..\..\Utilities\Parser.h"
#include "..\..\Utilities\Logger.h"
#include "..\..\Utilities\Constants.h"
class Client
{
public:

	Client()
	{
		//tymczasowe ID
		ID = -1;
	}

	void startClient();
private:
	void refreshClient();
	Parser::Messenger output;
	Parser::Messenger input;

	int ID;
	int time;
	std::string confName = "./data/config.txt";
	std::string playerName = "Player One";
	std::vector<std::string> playerList;

	std::string port = "7777";
	std::string IpAddress = "127.0.0.1";
	Network network;

	void startLogger();

	void mainLoop();
	void handleEvents(Parser::Messenger mess);
	void handleServer(Parser::Event ev);
	void handleLobby(Parser::Event ev);

	void handleInitPlayer(Parser::Event ev);
	void handleNewPlayer(Parser::Event ev);
	void handleTimeout(Parser::Event ev);
	void handleDisconnect(Parser::Event ev);

	void loadConfig();

	void processConfigLine(std::string line);

	void setConfigValue(std::string token, std::string value);






};

