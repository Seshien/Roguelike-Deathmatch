#pragma once
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include "..\..\Utilities\Event.h"
#include "..\..\Utilities\Network.h"
#include "..\..\Utilities\Parser.h"
#include "..\..\Utilities\Logger.h"
#include "..\..\Utilities\Constants.h"
#include "UIButton.h";

class Client
{
public:

	Client()
	{
		//tymczasowe ID
		ID = -1;
		cState = ConnectionState::NOTCONNECTED;
	}

	void startClient();

private:

	enum class ConnectionState {NOTCONNECTED, CONNECTED, FAILED};
	enum class GameStage {NOTJOINED, LOBBY, DEAD, ALIVE};
	void connectClient();
	void startWindow();


	sf::RenderWindow window;
	sf::RectangleShape rectangle;
	Parser::Messenger output;
	Parser::Messenger input;
	
	sf::View gameView;
	sf::View lobbyView;
	sf::View interfaceView;


	int ID;
	int time;
	int mapID;

	ConnectionState cState;
	GameStage gameStage;

	std::string confName = "./data/config.txt";
	std::string playerName = "Player One";
	std::vector<std::string> playerList;

	std::string port = "7777";
	std::string IpAddress = "127.0.0.1";
	Network network;

	//std::array<sf::Texture, 16> tileTextures;

	sf::Texture buttonTexture;

	UIButton getIn;

	sf::Clock turnTimer;

	void startLogger();

	void mainLoop();
	void handleNetEvents(Parser::Messenger mess);
	void handleIntEvents();
	void handleServer(Parser::Event ev);
	void handleLobby(Parser::Event ev);

	void handleInitPlayer(Parser::Event ev);
	void handleNewPlayer(Parser::Event ev);
	void handleTimeout(Parser::Event ev);
	void handleDisconnectPlayer(Parser::Event ev);
	void handleLostConnection(Parser::Event ev);
	void loadConfig();

	void processConfigLine(std::string line);

	void setConfigValue(std::string token, std::string value);

	void loadTileTextures();
	void loadObjectTextures();
	void loadUITextures();
	void graphicsUpdate();
};

